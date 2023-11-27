#include "tar_stuff.h"

void extract_files(int tarfile, Options *opts) {
    uint8_t buf[BLOCK_SIZE]; /* buffer for reading blocks */
        

    while (read(tarfile, buf, BLOCK_SIZE) > 0) {
        /* do stuff */
    }    
}
