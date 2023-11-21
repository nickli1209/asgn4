CC = gcc

LD = gcc

CFLAGS = -Wall -g 

LDFLAGS = -g

mytar: mytar.o tar_stuff.o archive.o
	$(LD) $(LDFLAGS) -o mytar mytar.o tar_stuff.o archive.o

mytar.o: mytar.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o mytar.o mytar.c

archive.o: archive.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o archive.o archive.c

tar_stuff.o: tar_stuff.c tar_stuff.h
	$(CC) $(CFLAGS) -c -o tar_stuff.o tar_stuff.c

clean:
	rm -f *.o mytar
