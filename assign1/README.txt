Programming Assignment 1
-----------------------------------------------------------------------------------------------
Files available
C files: dberror.c; storage_manager.c;test_assign1_1.c
Header files: storage_mgr.h, dberror.h, test_helper.h
Makefile

------------------------------------------------------------------------------------------------
Team Members:
Arapi Arjol ,M10280755, arapial@mail.uc.edu
Bakhshi Pavni, M10347876, bakhshpi@mail.uc.edu
Deshpande Vaidehi, M14014569, deshpavi@mail.uc.edu
Guangyuan(Frank) Li, M13516401, li2g2@mail.uc.edu
Wang, Xinjian,  M13758857  , wang5xj@mail.uc.edu

-------------------------------------------------------------------------------------------------
Goal:
The goal of this assignment is to implement a storage manager that will perform following tasks.
1)Reading blocks (pages) from a file on disk into memory
2)Writing blocks (pages) from memory to a file on disk
3)Dealing with fixed-sized blocks
4)Manipulating files i.e. Creating / Opening / Closing the files.

--------------------------------------------------------------------------------------------------
How to execute the code:
-------------------------
1) To run the Makefile, type:-
Make

This is run all the required object files,header files and c files to create a binary executable file.

2) To run the executable file, type and enter:

Make run

This will run the binary file testing all the methods defined.

3) Finally to remove all the object files and the binary file create type and enter:

Make clean

In the make file we are using clang -c to avoid the linker error.
-----------------------------------------------------------------------------------------------------------------------
Programming logic used:
---------------------------------
The main part is how to read the files stored on disk, from a specific starting point to an end point. 
We utilized C function fseek function to adjust the position where the pointer refers to, then using fread function to read the certain blocks into memory. 
Since two critical handles, namely FileHandle and PageHandle have already been programmed, we just need to attach the file streams to the FileHandle as required. 
And using PageHandle to control the number of page we are currently add for implementing the read and write functions.
------------------------------------------------------------------------------------------------------------------------------------------------------------------
Methods used in the assignment:
------------------------------
------------------------------

Methods for file handling:-
-------------------------
createPageFile()
This method creates a new file with given file name. It initialises the file with 1 page and saves it as a zero byte file.

openPageFile()
This method checks if a file exists, if it does it opens the file otherwise it returns error code RC_FILE_NOT_FOUND. 
Secondly, this method initializes the information present in the file handler 'SM_FileHandle'. 
It performs tasks like reading the total number of pages present in the file.

closePageFile()
This method closes the page file.

destroyPageFile()
This method destroys the page file

-------------------------------------------------------------------------------------------------------------------------------------
Read and Write Methods:-
-------------------------
Read Methods:-
_______________

readBlock()
This method reads a stream from a page from the file and stores it at the memory location pointed by SM_PageHandle(memPage). 
If the file is empty then it return an error 'Return RC_READ_NON_EXISTING_PAGE'

getBlockPos()
This method reads the current page position and stores it in currentPagePos to be used in other methods.

readFirstBlock()
This method reads the first block/page in the file by looking for start of the page using SEEK_SET in the fseek function. The it reads the initial 4096 bytes and finally closes the file.

readPreviousBlock() 
This method goes to the previous page using currentPagePos and reads it. If not found it returns an error  RC_READ_NON_EXISTING_PAGE.

readCurrentBlock() 
This method declares the current page using the getBlockPos function and then reads the current page.

readNextBlock()
This method goes to the next page from current page and reads it. If no next page is found it returns error RC_READ_NON_EXISTING_PAGE.

readLastBlock()
This method reads the last block/page of the file. The last block is identified using the fseek function. The stream from the last page is read and finally it closes the file.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Write Methods:-
-------------------

writeBlock()
This method opens the file in write mode and writes the block using an absolute position seeked from the beginning of the file. 


writeCurrentBlock()
This method declares the current page position and the calls the write block method to write the block using the current page position


appendEmptyBlock()
This method opens the file in write mode and seeks the end of the file, and then adds 1 page at the end increasing the total page numbers by 1. Finally it closes the file.	


ensureCapacity()
This method checks if the file is exceeding the total number of pages, if it is then using the appendEmptyBlock method it adds an empty page/block at the end to make sure it is within the pagenumbers.


-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------