#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"


void initStorageManager (void)
{}


/* createPageFile */
RC createPageFile (char *fileName)
{
    FILE *fp;                       // declare file pointer
    int size = PAGE_SIZE;    // declare page size and initialize it
    fp = fopen(fileName,"w");     // open the file, arguments can be char array or char pointer, same for mode
    fseek(fp,0,SEEK_SET);      // initialize file size as 1 page, 4096 bytes 
    int i;
    for(i=0; i<size; i++) {
        fputc(0,fp);
        fseek(fp,0,SEEK_END);
    }
    fclose(fp);
    return RC_OK;
}


RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
    FILE *fp; 
    fp = fopen(fileName,"r");
    if(fp==NULL){
        return RC_FILE_NOT_FOUND;    // If no file exists, return "not found" return code
    } else{
        fseek(fp,0,SEEK_END); //looks for the end of the file
        int size = ftell(fp);   // retrieve size of the file
        printf("%d",size);
        //initializing filehandler, assign corresponding values
        fHandle->fileName = fileName;
        fHandle->totalNumPages = size / PAGE_SIZE;
        fHandle->curPagePos = 0;
        fHandle->mgmtInfo = fp;
        return RC_OK;
    }
}

RC closePageFile (SM_FileHandle *fHandle)
{
    fclose(fHandle->mgmtInfo); //closes the file
    return RC_OK;
}

RC destroyPageFile (char *fileName)
{
    remove(fileName); //destroys the file
    return RC_OK;
}


// Reading part

RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE * fp;
    fp = fopen(fHandle->fileName, "r");
    fseek(fp, 0, SEEK_END);
    int pages = ftell(fp) / PAGE_SIZE;
    if (pages < pageNum) {                  // if there are not enough page to read, should report this error!
        return RC_READ_NON_EXISTING_PAGE;
    }
    else {
        fseek(fp, 0, pageNum * PAGE_SIZE);
        fread(memPage,sizeof(char),PAGE_SIZE,fp);   // read the stream from disk/file to memory address 
        fclose(fp);
        return RC_OK;
    }
}


int getBlockPos (SM_FileHandle *fHandle)
{
    int curPagePos = fHandle->curPagePos; //gets the current position 
    return curPagePos;
}



RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE * fp;
    fp = fopen(fHandle->fileName,"r");
    fseek(fp, 0, SEEK_SET); //looks for begining of the file
    fread(memPage,sizeof(char),PAGE_SIZE,fp); //reads the starting 4096 bytes
    fclose(fp);
    return RC_OK;
}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE * fp;
    fp = fopen(fHandle->fileName,"r");
    int curPagePos = getBlockPos(fHandle);
    int prePagePos = curPagePos - 1; //declaring the previous page
    if (prePagePos < 0) {
        return RC_READ_NON_EXISTING_PAGE; //return the respective error
    }
    else {
        RC code = readBlock(prePagePos,fHandle, memPage); //looking for previous page
        return code;
    }

}

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *fp;
    fp = fopen(fHandle->fileName,"r");
    int curPagePos = getBlockPos(fHandle); //declaring the current block position
    RC code = readBlock(curPagePos,fHandle, memPage); //gets the current block
    return code;

}

RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *fp;
    fp = fopen(fHandle->fileName,"r");
    int curPagePos = getBlockPos(fHandle);
    int nextPagePos = curPagePos + 1; //declaring the next block position
    if (nextPagePos > fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    else {
        RC code = readBlock(nextPagePos,fHandle, memPage); //gets the next block
        return code;
    }
}

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE * fp;
    fp = fopen(fHandle->fileName,"r");
    fseek(fp, 0, SEEK_SET - PAGE_SIZE); //seeks the last block
    fread(memPage,sizeof(char),PAGE_SIZE,fp);
    fclose(fp);
    return RC_OK;
}

// writing part

RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int i;
    FILE *fp;
    SM_PageHandle ph;
    fp = fopen(fHandle->fileName,"w"); //opens the file in write mode
    fseek(fp, 0, pageNum * PAGE_SIZE); // seeks a specific page from the start of the file
    for (i=0; i<PAGE_SIZE; i++) {
        fputc(memPage[i],fp);
        fseek(fp,0,SEEK_END);
    }
    printf("%ld",ftell(fp));
    fclose(fp);
    return RC_OK;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *fp;
    fp = fopen(fHandle->fileName,"w");
    int curPagePos = fHandle->curPagePos;
    RC code = writeBlock(curPagePos, fHandle, memPage); //writes the current page
    fHandle->curPagePos += 1;
    return code;
}

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    FILE *fp;
    fp = fopen(fHandle->fileName, "w");
    int size = PAGE_SIZE; 
    int increase_size = PAGE_SIZE;
    fseek(fp, increase_size, SEEK_END);
    fHandle->totalNumPages += 1 ;   // increase fileHandle totalNumPages by 1
    int i;
    for(i=0; i<size; i++) {
        fputc(0,fp);
        fseek(fp,0,SEEK_END);
    }
    fclose(fp);
    return RC_OK;
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
    if (fHandle->totalNumPages < numberOfPages) {   // basically, it can not exceed total pages
        RC code = appendEmptyBlock(fHandle);    // If so, append an empty blocks
        return code;
    }
    else {
        return RC_OK;
    }

}