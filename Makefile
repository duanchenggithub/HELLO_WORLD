OBJ = fun.o main.o 
CC = gcc
CFLAGS = -Wall -O0 -g -llog
all:$(OBJ) liblog.a
	$(CC) $(OBJ) -o test 
fun.o: head.h
main.o: head.h
liblog.a: log.o
	ar crs liblog.a log.o
log.o: log.h
.PHONY:clean
clean:
		rm -f test $(OBJ) liblog.a log.o
