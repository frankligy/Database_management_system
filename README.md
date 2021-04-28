# Database_management_system

C Implementation of a simple Database management system (DBMS), including storage manager and buffer manager.

# Disclaimer

This is for the programming assignment for University of Cincinnati 2021 Spring semester `CS 6051`, taught by [Dr. Lee](https://researchdirectory.uc.edu/p/lee5sk). This repository is mainly for my own future reference since this is my first time to program in C. Two .c files are entirely my work though, so I would be appreciated if you can note the source if you intend to use it

- `storage_mgr.c`: [Click Here](https://github.com/frankligy/Database_management_system/blob/main/assign1/storage_mgr.c)

- `buffer_mgr.c`: [Click Here](https://github.com/frankligy/Database_management_system/blob/main/assign2/buffer_mgr.c)

# Storage manager

A storage manager is basically to control how the data records are stored in disk, bundled with basic reading and writing function to achieve the transfer between disk and memory.

To compile and run:
```
cd ./assign1
make
./test_assign1
```

# Buffer manager

A buffer manager intends to control how the pages are aranged in memory, we define a buffer pool which contains a certain number of memory block. We need a buffer manager to control, where to position each page read from disk, if modified, how to label them and assure the changes will be written back to the disk.

To compile and run;
```
cd ./assign2
make
./test_assign2
```

# Blog

I wrote a blog for popularizing some fun facts of data storage and computing system. Check it out if you are interested in:
https://towardsdatascience.com/how-your-data-is-stored-on-disk-and-memory-8842891da52

# Contact

Guangyuan(Frank) Li

li2g2@mail.uc.edu

PhD student, Biomedical Informatics

Cincinnati Children's Hospital Medical Center(CCHMC)

University of Cincinnati, College of Medicine
