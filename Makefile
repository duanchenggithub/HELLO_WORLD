OBJ = fun.o main.o 
CC = gcc
CFLAGS = -Wall -O0 -g -I . -L . -llog
all:$(OBJ) log
	$(CC) $(OBJ) $(CFLAGS) -o test 
fun.o: head.h
main.o: head.h
log: log.o
	ar crs liblog.a log.o
log.o: log.h
.PHONY:clean
clean:
		rm -f test $(OBJ) liblog.a log.o
