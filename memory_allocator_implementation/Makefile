CC=gcc
CFLAGS=-fsanitize=address -Wall -Werror -std=gnu11 -g

tests: tests.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm

# compiles the tests and runs the tests
run_tests: testing1 testing2 testing3 testing4 testing5 testing6 testing7 testing8 testing9 testing10 testing11 testing12 testing13
	./run.sh
testing1: ./test_c_files/testing1.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing2: ./test_c_files/testing2.c virtual_alloc.c functions.c
		$(CC) $(CFLAGS) $^ -o $@ -lm
testing3: ./test_c_files/testing3.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing4: ./test_c_files/testing4.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing5: ./test_c_files/testing5.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing6: ./test_c_files/testing6.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing7: ./test_c_files/testing7.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing8: ./test_c_files/testing8.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing9: ./test_c_files/testing9.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing10: ./test_c_files/testing10.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing11: ./test_c_files/testing11.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing12: ./test_c_files/testing12.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
testing13: ./test_c_files/testing13.c virtual_alloc.c functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
