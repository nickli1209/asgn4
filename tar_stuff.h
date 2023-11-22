#ifndef TAR_STUFF_
#define TAR_STUFF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ON 1

#define NAME_OFFSET 0
#define MODE_OFFSET 100
#define GNAME_OFFSET 297
#define SIZE_OFFSET 124
#define MTIME_OFFSET 136


#define MAX_PATH 256
#define MAX_NAME 100
#define MAX_PREFIX 155


typedef struct{
    int c,t,x,v,f,S;
}Options;

typedef struct {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag;
	char linkname[100];
	char magic[6];  
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
}Header;

typedef struct Node Node;

struct Node{
	Header *header;
	struct Node *next;
};

Options *check_options(char *options);
Node *traverse_files(Node *head, char *path, Options *opts);
Node *insert_end(Node *head, Header *header);

/* Header stuff */
Header *create_header(char *name, struct stat *sb, Options *opts);
void pop_name(Header *header, char *fullpath);
void pop_IDs(Header *header, struct stat *sb, Options *opts);
void pop_typeflag(Header *header, struct stat *sb);
void pop_linkname(Header *header, char *path, struct stat *sb);

/* helpers */
void int_to_octal(char * dest, int size, unsigned long val);
int insert_special_int(char *where, size_t size, int32_t val);
uint32_t extract_special_int(char *where, unsigned int len);


#endif

