CC = gcc

LD = gcc

CFLAGS = -Wall -Wextra -g -ansi -pedantic -static -std=gnu99

LDFLAGS = -g

mytar: mytar.o tar_stuff.o archive.o list.o
	$(LD) $(LDFLAGS) -o mytar mytar.o tar_stuff.o archive.o list.o

mytar.o: mytar.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o mytar.o mytar.c

archive.o: archive.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o archive.o archive.c

list.o: list.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o list.o list.c

tar_stuff.o: tar_stuff.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o tar_stuff.o tar_stuff.c

clean:
	rm -f *.o *.tar mytar
