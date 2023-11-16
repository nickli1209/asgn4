#ifndef TAR_STUFF_
#define TAR_STUFF_

#include <stdio.h>
#include <stdlib.h>

#define ON 1
#define OFF 0


typedef struct{
    int c,t,x,v,f,S;
}Options;


Options *check_options(char *options);




#endif

