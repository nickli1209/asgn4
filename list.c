#include "tar_stuff.h"

/* TODO make this a function in tar_stuff.c - useful for reading headers
 * should read header one at a time and return it. This might be were a linked 
 * list is useful */
void contents(int tarfile, Options *opts) {
 	uint8_t buf[BLOCK_SIZE];
 	char fullpath[MAX_PATH];
  	unsigned long size;
  	int offset;
  	Header *header;
  	uint8_t check[BLOCK_SIZE];

  	memset(check, '\0', BLOCK_SIZE);
	while (read(tarfile, buf, BLOCK_SIZE) > 0) {
    		if (memcmp(buf, check, BLOCK_SIZE) == 0) {
			if(memcmp(buf, check, BLOCK_SIZE) == 0) {
				break;
			}	
    		}
    		/* fills header with header data from buf*/
		header = readHeader(buf);
   		size = strtoul(header->size,NULL,8); 
    		if (opts->v) {
      			print_perms(header->mode);
      			printf(" %s/%s        ", header->uname, header->gname);
      			printf("%8d ", (int) size);
      			time_t time = (time_t) strtoul(header->mtime, NULL, 8);
      			struct tm *timeinfo = localtime(&time);
      			char formatted_time[17];
      			strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M", timeinfo);
      			printf("%s ", formatted_time);
    		}
    
    		if (header->linkname[0] != '\0') {
      			printf("%s\n", header->linkname);
    		} else {
      		if (header->prefix[0] != '\0') {
        		snprintf(fullpath, MAX_PATH, "%s/%s", header->prefix, header->name);
      		} else {
        		strncpy(fullpath, header->name, MAX_NAME);
      		}
      			printf("%s\n", fullpath);
    		}
    		offset = size ? ((size / 512) + 1) : 0; /* calculate block to read */
    		lseek(tarfile, offset * 512, SEEK_CUR); /* seek to next header block */
  	}

  	close(tarfile);
  	return;
}

void print_perms(char *mode_octal) {
  	char perms[11];
  	unsigned long mode; 

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

