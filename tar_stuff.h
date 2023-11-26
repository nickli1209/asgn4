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
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <time.h>

#define ON 1

#define NAME_OFFSET 0
#define MODE_OFFSET 100
#define UID_OFFSET 108
#define GID_OFFSET 116
#define SIZE_OFFSET 124
#define MTIME_OFFSET 136
#define CHKSUM_OFFSET 148
#define TYPEFLAG_OFFSET 156
#define LINKNAME_OFFSET 157
#define MAGIC_OFFSET 257
#define VERSION_OFFSET 263
#define UNAME_OFFSET 265
#define GNAME_OFFSET 297
#define DEVMAJ_OFFSET 329
#define DEVMIN_OFFSET 337
#define PREFIX_OFFSET 345

#define MAX_PATH 256
#define MAX_NAME 100
#define MAX_MODE 8
#define MAX_ID 8
#define MAX_SIZE 12
#define MAX_MTIME 12
#define MAX_CHKSUM 8
#define MAX_TYPEFLAG 1
#define MAX_LINKNAME 100
#define MAX_MAGIC 6
#define MAX_VERSION 2
#define MAX_UNAME 32
#define MAX_GNAME 32
#define MAX_DEVMAJ 8
#define MAX_DEVMIN 8
#define MAX_PREFIX 155


#define BLOCK_SIZE 512

typedef struct{
    int c,t,x,v,f,S;
}Options;

typedef struct {
	char name[100];//			
	char mode[8];//
	char uid[8];//
	char gid[8];//
	char size[12];//
	char mtime[12];//
	char chksum[8];
	char typeflag[1];//
	char linkname[100];//
	char magic[6];  
	char version[2];
	char uname[32];//
	char gname[32];//
	char devmajor[8];
	char devminor[8];
	char prefix[155];//
	char padding[12];
}Header;

typedef struct Node Node;

struct Node{
	Header *header;
	struct Node *next;
};

void check_options(char *options, Options *opts);
void traverse_files(char *path, Options *opts, int tarfile);
Node *insert_end(Node *head, Header *header);

/* header stuff */
Header *create_header(char *name, struct stat *sb, Options *opts);
void pop_name(Header *header, char *fullpath);
void pop_IDs(Header *header, struct stat *sb, Options *opts);
void pop_typeflag(Header *header, struct stat *sb);
void pop_linkname(Header *header, char *path, struct stat *sb);
void pop_symnames(Header *header, struct stat *sb);
void pop_chksum(Header *header);
void pop_dev(Header *header, struct stat *sb);
void write_header(Header *header, char *path, int fd);
void write_end(int tarfile);

/* table of contents stuff */
void contents(int tarfile, Options *opts,char ** pathList);
void print_perms(char *mode);
Header *readHeader(uint8_t *buf);
/* helpers */
void int_to_octal(char * dest, int size, unsigned long val);
int insert_special_int(char *where, size_t size, int32_t val);
uint32_t extract_special_int(char *where, unsigned int len);


#endif

