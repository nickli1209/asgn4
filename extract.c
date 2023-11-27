#include "tar_stuff.h"

void extract_files(int tarfile, Options *opts) {
    uint8_t buf[BLOCK_SIZE]; /* buffer for reading blocks */
    uint8_t check[BLOCK_SIZE]; /* buf to check for 2 NULL blocks */
    char fullpath[MAX_PATH];
    int i, bytes, fd, offset;
    unsigned long size, ttl_rd;
    Header *header;
          

    while ((bytes = read(tarfile, buf, BLOCK_SIZE)) > 0) {
        if (memcmp(buf, check, BLOCK_SIZE) == 0) {
            if (read(tarfile, buf, BLOCK_SIZE) == -1) {
                perror("read on tarfile");
                exit(EXIT_FAILURE);
            }
            if (memcmp(buf, check, BLOCK_SIZE) == 0) {
                break;
            }
        }
        header = readHeader(buf);
        size = strtoul(header->size, NULL, 8);
    
		if (header->prefix[0] != '\0') {
            snprintf(fullpath, MAX_PATH, "%s/%s", header->prefix, header->name);
        } else {
            strncpy(fullpath, header->name, MAX_NAME);
        }
        
        if (opts->v) {
            printf("%s\n", fullpath);
        }

        fd = create_ent(fullpath, header);
        offset = size ? ((size / 512) + 1) : 0;
        ttl_rd = 0;
        while (ttl_rd < size) {
            if ((bytes = read(tarfile, buf, BLOCK_SIZE)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            } else {
                ttl_rd += bytes;
                if (ttl_rd >= size) {
                    int diff = ttl_rd - size;
                    if (write(fd, buf, BLOCK_SIZE - diff - 1) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                } else if (write(fd, buf, BLOCK_SIZE) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    } 
                }     

        }

        if (fd > 0) {
            close(fd);
        }

        /*lseek(tarfile, offset * 512, SEEK_CUR); */
        free(header);
    }

    if (bytes == -1) {
        perror("read on tarfile");
        exit(EXIT_FAILURE);
    }
    close(tarfile);
    return;
}

int create_ent(char *fullpath, Header *header) {
    mode_t mode; /* mode from header */
    mode_t perms; /* perms for files */
    int fd;

    mode = (mode_t) strtoul(header->mode, NULL, 8);
    perms = mode;
    if (S_ISDIR(mode) || ((S_IXUSR | S_IXGRP | S_IXOTH) & mode) != 0) {
        perms |= (S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
        perms |= (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    }
    
    if (S_ISDIR(mode)) {
        if ((fd = mkdir(fullpath, perms)) == -1) {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    } else if ((fd = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC, perms)) == -1) {
        perror("open");
        //fprintf(stderr, "failed to open %s", fullpath);
        exit(EXIT_FAILURE);
    }
    return fd;
}
