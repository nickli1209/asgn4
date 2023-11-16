CC = gcc

LD = gcc

CFLAGS = -Wall -Wextra -g -ansi -pedantic -static

LDFLAGS = -g

mytar: mytar.o tar_stuff.o
	$(LD) $(LDFLAGS) -o mytar mytar.o tar_stuff.o

mytar.o: mytar.c
	$(CC) $(CFLAGS) -c -o mytar.o mytar.c

tar_stuff.o: tar_stuff.c
	$(CC) $(CFLAGS) -c -o tar_stuff.o tar_stuff.c

clean:
	rm -f *.o mytar
