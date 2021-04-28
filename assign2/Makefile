
test_assign2: test_assign2_1.o buffer_mgr.o buffer_mgr_stat.o dberror.o storage_mgr.o
	clang test_assign2_1.o buffer_mgr_stat.o buffer_mgr.o dberror.o storage_mgr.o -o test_assign2

test_assign2_1.o: test_assign2_1.c storage_mgr.h buffer_mgr_stat.h buffer_mgr.h dberror.h test_helper.h
	clang -c test_assign2_1.c

buffer_mgr.o: buffer_mgr.c buffer_mgr.h storage_mgr.h dt.h
	clang -c buffer_mgr.c

buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h buffer_mgr.h
	clang -c buffer_mgr_stat.c

dberror.o: dberror.c dberror.h
	clang -c dberror.c

storage_mgr.o: storage_mgr.c dberror.h storage_mgr.h
	clang -c storage_mgr.c


clean: 
	rm -rf test_assign2 *.o *.bin