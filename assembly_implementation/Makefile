CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan -lm

# fill in all your make rules

#DEPENDENCIES="./cfiles/functions.c ./headerfiles/functions.h ./cfiles/list.c ./headerfiles/list.h"
vm_x2017: vm_x2017.c ./cfiles/functions.c ./headerfiles/functions.h ./cfiles/list.c ./headerfiles/list.h
	${CC} ${CFLAGS} vm_x2017.c ./cfiles/functions.c ./cfiles/list.c -o vm_x2017

objdump_x2017: objdump_x2017.c ./cfiles/functions.c ./headerfiles/functions.h ./cfiles/list.c ./headerfiles/list.h
	${CC} ${CFLAGS} objdump_x2017.c ./cfiles/functions.c ./cfiles/list.c -o objdump_x2017

tests:
	"tests"

run_tests:
	./test.sh

clean:
	"clean"
