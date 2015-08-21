OBJ = fun.o main.o
CC = gcc
CFLAGS = -Wall -O0 -g 
all:$(OBJ) 
	$(CC) $(OBJ) -o $@ 
fun.o: head.h
main.o: head.h
.PHONY:clean
clean:
		rm -f test $(OBJ)
	

