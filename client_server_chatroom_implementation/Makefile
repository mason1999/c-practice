CC=gcc
CFLAGS=-Wall -Werror -Wvla -std=gnu11 -fsanitize=address
PFLAGS=-fprofile-arcs -ftest-coverage
DFLAGS=-g
HEADERS=server.h functions.h
SRC=server.c functions.c

procchat: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $@

test:
	$(CC) $(CFLAGS) $(PFLAGS) $(SRC) -o $@


make_tests:
	echo "no tests available"

clean:
	rm -f procchat

