## This is PA2

The code is completed by Guangyuan(Frank) Li.
 

## Explanation of function implemented in buffer_mgr.c

- initBufferPool: initiate the buffer pool, in order to demarcate each page frame, here I first declare a struct called "Bag", this struct serve as a bag to hold all the auxiliary information needed to keep track of each page frame, including the actually memory address, page number, whether dirty or not, fixCount and a running index recording how often the page was pineed which will later be used in LRU implementation.

- getFrameContent: it is as simple as reading each page frame, append the page number variable to a newly defined array.

- getDiryPage: the same concept as getFrameContent.

- getFixCount: the same concept as getFrameContent.

- getNumReadIO: we define a global variable readIO to record how many times the page has been read from the disk.

- getNumWriteIO: we define a global variable writeIO to record how many tiems tha page has been written back to the disk.

- inCache: this function is an auxiliary function to check is a page exists in buffer or not.

- isFull: this function is an auxiliary function to check whether the pool is full or not.

- FIFO_strategy: this function implement the FIFO strategy. The key here is the fifo_remove_index variable, it is a globally defined variable that keep track of at each time when FIFO strategy function get invoked, which page frame needs to be released. Here we take the remainder of last remove_index by dividing the numPages.

- LRU_strategy: this function implement the LRU strategy. Here I used a novel strategy, I set up two global variable, one is pin_num, which records the number of time that pages have been pinned, then a lru_slot which stores the orders of least recently used pages. At one time point, for instance, the lru_slot is [3,4,5,2,1], it means the 3 should be the one that get removed in this time point.

- pinPage: first, I read the corresponding page from the disk to a temporaray memory called tmp_reading. Then this function basically checks (a) whether the page is in the frame, (b) whether the page frame is fool or not. Based on these two condition, I either directly put the page->data pointer to the corresponding address already in the memory buffer, or actually first redirect the address of tmp_reading to the memory buffer.

- markDiry: check all the page order, mark the one we just pinned as dirty.

- unpinPage: check all the fixCount, make the fixCount minus one for the page we are now referring to.

- forcePage: forcefully write the current content in the memory back to disk

- shutDownBuffer: it calls for the next function, forceFlushPool to first write all the page frame to the disk, then it just release the memory space.

- forceFlushPool: if the page frame is diry, write it back to the disk.


## Contact

Guangyuan(Frank) Li

li2g2@mail.uc.edu

PhD student, Biomedical Informatics

Cincinnati Children's Hospital Medical Center(CCHMC)

University of Cincinnati, College of Medicine
