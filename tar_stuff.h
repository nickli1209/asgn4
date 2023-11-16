#ifndef TAR_STUFF_
#define TAR_STUFF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ON 1
#define OFF 0

#define name_offset 0
#define mode_offset 100
#define gname_offset 297
#define size_offset 124
#define mtime_offset 136


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
void traverse_files(char *path, int path_index, Options *opts);



#endif

