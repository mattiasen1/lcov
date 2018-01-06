test: test.c allocate.o test.o refmem.o
	gcc -ggdb test.c allocate.o  refmem.o -o test -g

tests: tests.c allocate.o refmem.o 
	gcc -g tests.c allocate.o refmem.o -o tests -g -lcunit

#ltest: test_allocate.c allocate.c
#	gcc test_allocate.c allocate.c -Wall -o ltest -g -lcunit

#ttest: test_rc.c rc.c
#	gcc test_rc.c rc.c -Wall -o ttest -g -lcunit

coverage: tests.c refmem.c allocate.c
	gcc -std=c11 -Wall -fprofile-arcs -ftest-coverage tests.c refmem.c allocate.c -o cov -lcunit
	./cov
	gcov allocate.c
	cat allocate.c.gcov

showcoverage: tests.c refmem.c allocate.c
	gcc -std=c11 -Wall -fprofile-arcs -ftest-coverage tests.c refmem.c allocate.c -o cov -lcunit
	./cov
	lcov --capture --directory ~/src --output-file coverage.info
	genhtml coverage.info --output-directory out

allocate.o: allocate.c
	gcc allocate.c -c -g


refmem.o: refmem.c refmem.h
	gcc refmem.c -Wall -c -g

run: test
	./test

debug: test
	gdb test

valgrind: test
	valgrind --leak-check=full ./test

runtests: tests
	valgrind --leak-check=full ./tests

debugtester: tester
	gdb tester

valgrindtester: tester
	valgrind --leak-check=full ./tester

runltest: ltest
	./ltest

runttest: ttest
	./ttest

clean:
	rm -f test allocate.o refmem.o test.o  

.PHONY: run debug clean valgrind runtester debugtester valgrindtester runltest runttest
