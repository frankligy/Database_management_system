test_assign1: dberror.o storage_mgr.o test_assign1_1.o
	clang test_assign1_1.o dberror.o storage_mgr.o -o test_assign1

dberror.o: dberror.c dberror.h
	clang -c dberror.c

storage_mgr.o: storage_mgr.c storage_mgr.h dberror.h
	clang -c storage_mgr.c

test_assign1_1.o: test_assign1_1.c test_helper.h dberror.h storage_mgr.h
	clang -c test_assign1_1.c


run: 
	./test_assign1

clean:
	rm -rf test_assign1 *.o
