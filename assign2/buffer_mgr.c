#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dt.h"

// we need to declare a struct
struct Bag {
    char *memoryFrame;
    int pageOrder;
    bool isDirty;
    int fixCount;
    int lru_index;
};

// we need to define two counter, readIO and writeIO
int readIO = 0;
int writeIO = 0;

// we need to define some helper variable
int fifo_remove_index = 0;
int pin_num = 0;
int lru_slot[5];




// initialize the buffer pool
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		const int numPages, ReplacementStrategy strategy,
		void *stratData) {
            bm->pageFile = (char *)pageFileName;  // char pointer
            bm->numPages = numPages;   // int
            bm->strategy = strategy;   // ReplacementStrategy, which is enum


            struct Bag *arr_bag = malloc (sizeof(struct Bag) * numPages);
            int i;
            for (i=0; i<numPages; i++) {
                arr_bag[i].memoryFrame = malloc (PAGE_SIZE);
                arr_bag[i].pageOrder = NO_PAGE;
                arr_bag[i].isDirty = FALSE;
                arr_bag[i].fixCount = 0;
                arr_bag[i].lru_index = 0;
            }

            bm->mgmtData = (struct Bag *) arr_bag;
            return 0;
        }

// statistic interface
PageNumber *getFrameContents (BM_BufferPool *const bm) {  
    struct Bag *tmpArray = (struct Bag *) bm->mgmtData;
    int *cacheArray = malloc (sizeof(int) * bm->numPages);
    int i;
    for (i=0; i<bm->numPages; i++) {
        cacheArray[i] = tmpArray[i].pageOrder;
        
    }
    return cacheArray;
}

bool *getDirtyFlags (BM_BufferPool *const bm) {
    struct Bag *tmpArray = (struct Bag *) bm->mgmtData;
    bool *dirtyArray = malloc (sizeof(bool) * bm->numPages);
    int i;
    for (i=0; i<bm->numPages; i++) {
        dirtyArray[i] = tmpArray[i].isDirty;
    }
    return dirtyArray;
}


int *getFixCounts (BM_BufferPool *const bm) {
    struct Bag *tmpArray = (struct Bag *) bm->mgmtData;
    int *fixCountsArray = malloc (sizeof(int) * bm->numPages);
    int i;
    for (i=0; i<bm->numPages; i++) {
        fixCountsArray[i] = tmpArray[i].fixCount;
    }
    return fixCountsArray;
}

int getNumReadIO (BM_BufferPool *const bm) {
    return readIO;
}

int getNumWriteIO (BM_BufferPool *const bm) {
    return writeIO;
}


// key function
int inCache(BM_BufferPool *const bm, const PageNumber pageNum) {
    int i;
    int *tmp = getFrameContents(bm);
    for (i=0; i<bm->numPages; i++) {
        if (tmp[i]==pageNum) {
            return i;
        }
    }
    free(tmp);
    return -1;
}

int isFull(BM_BufferPool *const bm) {
    int i;
    int *tmp = getFrameContents(bm);
    for (i=0; i<bm->numPages; i++) {
        if (tmp[i]==NO_PAGE) {
            return i;
        }
    }
    free(tmp);
    return -1;
}

void FIFO_strategy(BM_BufferPool *const bm, BM_PageHandle *const page, char *const tmp_reading) {
    struct Bag *tmpArray = (struct Bag *)bm->mgmtData;

    if (tmpArray[fifo_remove_index].isDirty == TRUE) {
        // write to disk
        SM_FileHandle fh;
        openPageFile(bm->pageFile,&fh);
        //printf("write to:%d\n",tmpArray[fifo_remove_index].pageOrder);
        //printf("write what:%s\n",tmpArray[fifo_remove_index].memoryFrame);
        writeBlock(tmpArray[fifo_remove_index].pageOrder, &fh, tmpArray[fifo_remove_index].memoryFrame);
        writeIO++;
    }

    // accomodate new element
    tmpArray[fifo_remove_index].memoryFrame = tmp_reading;
    page->data = tmpArray[fifo_remove_index].memoryFrame;
    tmpArray[fifo_remove_index].pageOrder= page->pageNum;
    tmpArray[fifo_remove_index].isDirty = FALSE;
    tmpArray[fifo_remove_index].fixCount++;
    // update fifo_remove_index
    fifo_remove_index = (fifo_remove_index + 1)%bm->numPages;


}

void LRU_strategy(BM_BufferPool *const bm, BM_PageHandle *const page, char *const tmp_reading) {

    // determine the least recent use page frame
    int lru_remove_index = lru_slot[pin_num%5];

    // actual part
    struct Bag *tmpArray = (struct Bag *) bm->mgmtData;  
    if (tmpArray[lru_remove_index].isDirty == TRUE) {
        // write to disk
        SM_FileHandle fh;
        openPageFile(bm->pageFile,&fh);
        //printf("write to:%d\n",tmpArray[lru_remove_index].pageOrder);
        //printf("write what:%s\n",tmpArray[lru_remove_index].memoryFrame);
        writeBlock(tmpArray[lru_remove_index].pageOrder, &fh, tmpArray[lru_remove_index].memoryFrame);
        writeIO++;
    }

    // accomodate new element
    tmpArray[lru_remove_index].memoryFrame = tmp_reading;
    page->data = tmpArray[lru_remove_index].memoryFrame;
    tmpArray[lru_remove_index].pageOrder= page->pageNum;
    tmpArray[lru_remove_index].isDirty = FALSE;
    tmpArray[lru_remove_index].fixCount++;


}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
		const PageNumber pageNum) {

            page->pageNum = pageNum; // int




            // check if requested page is in cache
            struct Bag *tmpArray = (struct Bag *) bm->mgmtData;  
            int result = inCache(bm,pageNum);
            if (result==-1) {  // requested page is not in cache

                SM_FileHandle fh;
                openPageFile(bm->pageFile, &fh);

                char *tmp_reading = malloc (PAGE_SIZE);
                //printf("total page: %d",fh.totalNumPages);
                ensureCapacity(pageNum, &fh);
                //printf("total page: %d\n",fh.totalNumPages);
                //printf("which page to read: %d\n",pageNum);
                readBlock(pageNum,&fh,tmp_reading);
                readIO++;
                //printf("read what:%s\n",tmp_reading);

                int full = isFull(bm);
                if (full!=-1) {  // pool is not full
                    tmpArray[full].memoryFrame = tmp_reading;
                    page->data = tmpArray[full].memoryFrame;
                    tmpArray[full].pageOrder = pageNum;
                    tmpArray[full].fixCount++;
                    lru_slot[pin_num%5] = full;
                }
                else {  // pool is full, need employ replacement strategy
                    if (bm->strategy == RS_FIFO) {
                        if (tmpArray[fifo_remove_index].fixCount==0) {
                            FIFO_strategy(bm,page,tmp_reading);
                        }                       
                        else {
                            fifo_remove_index = (fifo_remove_index + 1)%bm->numPages;
                            FIFO_strategy(bm,page,tmp_reading);
                            }
                        }
                    else if (bm->strategy == RS_LRU) {LRU_strategy(bm,page,tmp_reading);}

                }
            }

            else {  // requested page is in cache
                page->data = tmpArray[result].memoryFrame;
                tmpArray[result].fixCount++;
                tmpArray[result].pageOrder = pageNum;
                lru_slot[pin_num%5] = result;
            }

            pin_num++;   // global pin_num++
            return RC_OK;
        }


RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page) {
    struct Bag *tmpArray = (struct Bag *)bm->mgmtData;
    int i;
    for (i=0; i<bm->numPages; i++) {
        if (tmpArray[i].pageOrder == page->pageNum) {
            tmpArray[i].isDirty = TRUE;
            return RC_OK;
        }
    }
    return RC_OK;
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page) {
    struct Bag *tmpArray = (struct Bag *)bm->mgmtData;
    int i;
    for (i=0; i<bm->numPages; i++) {
        if (tmpArray[i].pageOrder == page->pageNum) {
            tmpArray[i].fixCount--;
            return RC_OK;
        }
    }
    return RC_OK;
}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {  // write the current content of page back to the page file on disk
    struct Bag *tmpArray = (struct Bag *)bm->mgmtData;
    int i;
    for (i=0; i<bm->numPages; i++) {
        if (tmpArray[i].pageOrder == page->pageNum) {
            SM_FileHandle fh;
            openPageFile(bm->pageFile, &fh);
            writeBlock(tmpArray[i].pageOrder,&fh,tmpArray[i].memoryFrame);
            tmpArray[i].isDirty = FALSE;
            writeIO++;
        }
    }
    return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const bm) {
    forceFlushPool(bm);
    fifo_remove_index = 0;
    readIO = 0;
    writeIO = 0;
    free(bm->mgmtData);
    return RC_OK;
}

RC forceFlushPool(BM_BufferPool *const bm) {
    struct Bag *tmpArray = (struct Bag *)bm->mgmtData;
    int i;
    for (i=0; i<bm->numPages; i++) {
        if (tmpArray[i].fixCount==0 && tmpArray[i].isDirty == TRUE) {
            SM_FileHandle fh;
            openPageFile(bm->pageFile,&fh);
            //printf("write to:%d\n",tmpArray[i].pageOrder);
            //printf("write what: %s\n",tmpArray[i].memoryFrame);
            writeBlock(tmpArray[i].pageOrder, &fh, tmpArray[i].memoryFrame);
            tmpArray[i].isDirty = 0;
            writeIO++;
        }
    }
    return RC_OK;
}






