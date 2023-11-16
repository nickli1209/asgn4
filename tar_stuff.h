#ifndef TAR_STUFF_
#define TAR_STUFF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ON 1
#define OFF 0

#define PATH_MAX 255

typedef struct{
    int c,t,x,v,f,S;
}Options;


Options *check_options(char *options);
void traverse_files(char *path, int path_index, Options *opts);



#endif

