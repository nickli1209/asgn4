#include "tar_stuff.h"

/* reads headers of tarfile and prints paths. if v option set, 
 * prints more info. if pathlist not NULL, prints only paths starting from 
 * those in pathlist*/
void contents(int tarfile, Options *opts,char ** pathList) {
 	uint8_t buf[BLOCK_SIZE]; /* buffer for block of tarfile */
 	char fullpath[MAX_PATH]; /* for concatenating name and prefix */
  	unsigned long size; /* for converting from octal to int */
  	int offset; /* offset in tarfile */
  	Header *header; /* header struct */
  	uint8_t check[BLOCK_SIZE]; /* check for 2 NULL blocks at end */
    int prntpths; /* flag for printing paths */

  	memset(check, '\0', BLOCK_SIZE);
    
    /* read from tarfile in blocks of 512 bytes */
	while (read(tarfile, buf, BLOCK_SIZE) > 0) {
        /* if we have read 2 blocks of NULL chars, we are done */
        if (memcmp(buf, check, BLOCK_SIZE) == 0) {
            if (read(tarfile, buf, BLOCK_SIZE) == -1) {
                perror("read on tarfile (contents)");
                exit(EXIT_FAILURE);
            }
			if(memcmp(buf, check, BLOCK_SIZE) == 0) {
				break;
			}
        }

        /* fill header struct with info read from header blocks */
		header = readHeader(buf);
        /* convert octal size to an integer for calculating offset */
   		size = strtoul(header->size,NULL,8);
        
        /* store full path for checking and printing */
		if (header->prefix[0] != '\0') {
            snprintf(fullpath, MAX_PATH, "%s/%s", header->prefix, header->name);
        } else {
            strncpy(fullpath, header->name, MAX_NAME);
        } 
        /* prntpths ON means we will print paths - turn it on (default),
         * then turn off if there are path args detected */
        prntpths = ON;
        if (pathList != NULL) {
            int i;
            int path_len;
            prntpths = OFF;
            /* iterate through path args,
             * if one matches turn prntpths back ON */
            for (i = 0; pathList[i] != NULL; i++) {
                path_len = strlen(pathList[i]);
                if (strncmp(fullpath, pathList[i], path_len) == 0) {
                    prntpths = ON;
                }
            } 
        }
		
        /* if prntpths ON, print everything */
        if (prntpths) {
            /* if verbose, print additional info */
            if (opts->v) {
                print_perms(header->mode); /* permissions */
                printf(" %s/%s     ", header->uname, header->gname); /* names */
                printf("%8d ", (int) size); /* size */
                /* time */
                time_t time = (time_t) strtoul(header->mtime, NULL, 8);
                struct tm *timeinfo = localtime(&time);
                char timestr[17];
                strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M", timeinfo);
                printf("%s ", timestr);
            }
            
            /* if we can, print linkname, else print path */
            if (header->linkname[0] != '\0') {
                printf("%s\n", header->linkname);
            } else {
                printf("%s\n", fullpath);
            }
        }

        offset = size ? ((size / 512) + 1) : 0; /* calculate block to read */
        lseek(tarfile, offset * 512, SEEK_CUR); /* seek to next header block */
		free(header); /* free header struct */
  	}

  	close(tarfile); /* close tarfile */
  	return;
}

/* takes an string containing the octal mode,
 * formats and prints perms */
void print_perms(char *mode_octal) {
  	char perms[11]; /* string to print */
  	unsigned long mode; /* actual mode */

    /* convert octal string to an unsigned long*/
  	mode = strtoul(mode_octal, NULL, 8);
  	if (S_ISDIR(mode)) {
    		perms[0] = 'd';
  	} else if (S_ISLNK(mode)) {
    		perms[0] = 'l';
  	} else {
  	  	perms[0] = '-';
  	}
  	perms[1] = (mode & S_IRUSR) ? 'r' : '-';
  	perms[2] = (mode & S_IWUSR) ? 'w' : '-';
  	perms[3] = (mode & S_IXUSR) ? 'x' : '-';
  	perms[4] = (mode & S_IRGRP) ? 'r' : '-';
  	perms[5] = (mode & S_IWGRP) ? 'w' : '-';
  	perms[6] = (mode & S_IXGRP) ? 'x' : '-';
  	perms[7] = (mode & S_IROTH) ? 'r' : '-';
  	perms[8] = (mode & S_IWOTH) ? 'w' : '-';
  	perms[9] = (mode & S_IXOTH) ? 'x' : '-';
  	perms[10] = '\0';
  	printf("%s", perms);
}

