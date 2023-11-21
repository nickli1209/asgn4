#ifndef TAR_STUFF_
#define TAR_STUFF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define ON 1
#define OFF 0

#define NAME_OFFSET 0
#define MODE_OFFSET 100
#define GNAME_OFFSET 297
#define SIZE_OFFSET 124
#define MTIME_OFFSET 136


#define PATH_MAX 255


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


Options *check_options(char *options);
void traverse_files(char *path, Options *opts);



#endif

