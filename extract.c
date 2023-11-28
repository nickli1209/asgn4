#include "tar_stuff.h"

void extract_files(int tarfile, Options *opts,char **pathList) {
    uint8_t buf[BLOCK_SIZE]; /* buffer for reading blocks */
    uint8_t check[BLOCK_SIZE]; /* buf to check for 2 NULL blocks */
    char fullpath[MAX_PATH];
    int bytes, fd,flag, vflag;
    unsigned long size;
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
	printf("name from heder:%s\n",header->name);
	/* verifies header is correct otherwise continues end here
	if(!checkChksum(header)){
	    fprintf(stderr,"ERROR: Invalid chksum detected,gave up extract");
	    free(header);
	    exit(EXIT_FAILURE);
	}*/	
	/*if strict is set and magic or version dont conform,skip*/
        if(opts->S){
	    if(strncmp(header->magic,"ustar",6!=0) ||
	    header->version[0]!='0' ||header->version[1]!='0'){
	        /*quits extract if magic and version dont conform*/
		fprintf(stderr,"ERROR: Invalid magic or version detected,gave up extract");
                free(header);
                exit(EXIT_FAILURE);
	    }
	}
	
	if (header->prefix[0] != '\0') {
            snprintf(fullpath, MAX_PATH, "%s/%s", header->prefix, header->name);
        } else {
            strncpy(fullpath, header->name, MAX_NAME);
        }
	
	flag = ON;
	vflag =ON;
        if (pathList != NULL) {
            int i;
            int path_len;
            flag= OFF;
	    vflag = OFF;
            /* iterate through path args,
 	    ** if one matches turn prntpths back ON */
            for (i = 0; pathList[i] != NULL; i++) {
                path_len = strlen(pathList[i]);
                if (strncmp(fullpath, pathList[i], path_len) == 0) {
			vflag = ON;
			flag = ON;
		}
		if (strncmp(fullpath, pathList[i], strlen(fullpath)) == 0) {
                    flag = ON;
                }
            }
        }
        if(flag){
            if (opts->v && vflag) {
            	printf("%s\n", fullpath);
            }
	
            fd = create_ent(fullpath, header);
	    if (size>0){
	        writeContents(fd,size,buf,tarfile);
	    }

	    /*set certain file stast from header fields*/
	    /*set times*/
	    struct utimbuf new_time;
	    struct stat fst;
	    if(stat(fullpath,&fst)==-1){
	       perror("stat");
	       exit(EXIT_FAILURE);
	    }
	    new_time.actime=fst.st_atime;	
	    new_time.modtime=(time_t) strtoul(header->mtime,NULL,8);
	    if(utime(fullpath,&new_time)==-1){
	        perror("utime");
	        exit(EXIT_FAILURE);   
	    }		
	    /*done utime*/	
	} else {
		/*skipToHeader(size, tarfile, buf);*/
        int offset;
        offset = size ? ((size / 512) + 1) : 0; /* calculate block to read */
        lseek(tarfile, offset * 512, SEEK_CUR); /* seek to next header block */
            
	}
	
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
    perms = 0;
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
    }else if(S_ISLNK(mode)){
	if((fd= symlink(header->linkname,fullpath))==-1){
	    perror("symlink");
	    exit(EXIT_FAILURE);
	}
    }else {
        if ((fd = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC, perms)) == -1) {
            fprintf(stderr, "failed to open %s", fullpath);
            exit(EXIT_FAILURE);
            }
    }
    return fd;
}
