#include "tar_stuff.h"

/* takes head of linked list, path string, and options,
 * does a preorder DFS of file tree. returns head of linked
 * list of Header structs */
void traverse_files(char *path, Options *opts, int tarfile) {
	DIR *dir; /* current directory */
	struct dirent *ent; /* entries inside dir */
	struct stat sb; /* stat buffer for entries */
	char fullpath[MAX_PATH]; /* holds full path */
	Header *header; /* header info */

	/* store entry info into sb */
	if (lstat(path, &sb) == -1) {
		perror("lstat");
		exit(EXIT_FAILURE); /* prob shouldn't exit here */
	}
	
	snprintf(fullpath, MAX_PATH, "%s/", path);
	/* if verbose option on, print current directory path*/
	if (opts->v) {	
		printf("%s\n", fullpath);
	}
	header = create_header(fullpath, &sb, opts);
	write_header(header, fullpath, tarfile);

	/* open current directory */
	if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(EXIT_FAILURE); /*prob don't wan't to exit here.*/
	}
	
	/* iterate through current's entries */
	while ((ent = readdir(dir)) != NULL) {
		/* skip over '.' and '..' entries  */
		if (strcmp(ent->d_name, ".") != 0 && 
		strcmp(ent->d_name, "..") != 0) {
				/* check to make sure path isn't too long
				 * before creating it*/
                if (strlen(path) + strlen(ent->d_name) > MAX_PATH) {
                    fprintf(stderr, "pathname over 256 characters");
				    exit(EXIT_FAILURE);
				}
				/* format new path, store in fullpath */
				snprintf(fullpath, MAX_PATH, "%s/%s",
					   	path, ent->d_name);

				/* store entry info into sb */
				if (lstat(fullpath, &sb) == -1) {
					perror("lstat");
					closedir(dir);
					exit(EXIT_FAILURE);
				}

				/* if it's a directory, recurse */
                if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
					traverse_files(fullpath, opts, tarfile);
				} else {
					/* else print path if verbose */
                    header = create_header(fullpath, &sb, opts);
					write_header(header, fullpath, tarfile);
					if (opts->v) {
						printf("%s\n", fullpath);
					}
				}
			}
	}
	closedir(dir);
	return;
}


Header *create_header(char *name, struct stat *sb, Options *opts) {
	Header *header;
	if ((header = malloc(sizeof(Header))) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* helps w/ NULL terminating */
	memset(header, '\0', sizeof(Header));

	pop_name(header, name); /* name */
    int_to_octal(header->mode, sizeof(header->mode),
            sb->st_mode); /* mode */
	pop_IDs(header, sb, opts); /* uid and gid */
	/* size */
	if (!S_ISLNK(sb->st_mode) && !S_ISDIR(sb->st_mode)) {
		int_to_octal(header->size, sizeof(header->size), sb->st_size);
	} else {
		int_to_octal(header->size, sizeof(header->size), 0);
	}
	int_to_octal(header->mtime, sizeof(header->mtime),
		(unsigned int)sb->st_mtime); /* mtime */
	pop_typeflag(header, sb); /* typeflag */
	pop_linkname(header, name, sb); /* linkname */
	strcpy(header->magic, "ustar"); /* magic */
	strncpy(header->version, "00", 2); /* version (NOT NULL terminated) */
	pop_symnames(header, sb); /* uname and gname */
	pop_dev(header, sb); /* devmajor, devminor */
	pop_chksum(header); /* checksum */
	return header;
}

/* takes Header and pathname string, populates header
 *  name and prefix with pathname */
void pop_name(Header *header, char *fullpath) {
  unsigned int index; /* current index, used if prefix is needed */

	/* if the length is 100 chars or less, copy it to name
	 * we can use strlen since NULL char is optional */
  if (strlen(fullpath) <= MAX_NAME) {
    strncpy(header->name, fullpath, strlen(fullpath));
  } else {
		/* else move back 100 chars, then move forward until
		 * encountering a slash OR getting to end of string */
    index = strlen(fullpath) - MAX_NAME - 1;
    while (fullpath[index] != '/') {
			/* if the end is reached, throw an error */
      if (index >= strlen(fullpath)-1) {
        fprintf(stderr, "pathname too long");
        exit(EXIT_FAILURE);
      }
      index++;
    }
    index++;
		/* use index to seperate which strings are copied to 
		 * name and prefix strings */
    strncpy(header->name, &fullpath[index], strlen(fullpath) - index);
    strncpy(header->prefix, fullpath,
            index-1); /* -1 since we don't need the slash */
  }
  return;
}

/* takes header struct, stat buffer, and options struct, 
 * populates header with user/group IDs */
void pop_IDs(Header *header, struct stat *sb, Options *opts) {
	if (sb->st_uid <= 07777777) {
		/* if the st_uid can fit into 7 octal digits,
         * create octal string and throw it in the header */
		int_to_octal(header->uid, sizeof(header->uid), sb->st_uid);
	} else if (!opts->S) {
		/* else if S option not ON, use insert_special_int */
        insert_special_int(header->uid, sizeof(header->uid), sb->st_uid);
	} else {
		/* otherwise, throw error */
		fprintf(stderr,
                "user ID does not fit in header: turn off S option\n");
		exit(EXIT_FAILURE);
	}
	/* likewise for group IDs */
	if (sb->st_gid <= 07777777) {
		int_to_octal(header->gid, sizeof(header->gid), sb->st_gid);
	} else if (!opts->S) {
        insert_special_int(header->gid, sizeof(header->gid), sb->st_gid);
	} else {
		fprintf(stderr,
                "group ID does not fit in header: turn off S option\n");
		exit(EXIT_FAILURE);
	}
	return;
}

/* takes header struct and stat buffer, populates 
 * typeflag field */
void pop_typeflag(Header *header, struct stat *sb) {
	if (S_ISREG(sb->st_mode)) {
		header->typeflag[0] = '0'; /* if regular, set to '0' */
	} else if (S_ISLNK(sb->st_mode)) {
		header->typeflag[0] = '2'; /* if symlink, set to '2' */
	} else if (S_ISDIR(sb->st_mode)) {
		header->typeflag[0] = '5'; /* if directory, set to '5' */
	} else {
		/* else regular file (alternate), so set to NULL */
		header->typeflag[0] = '\0'; 
	}
	return;
}

/* takes header struct, pathname, and stat buffer, 
 * populates linkname field */
void pop_linkname(Header *header, char *path, struct stat *sb) {
	/* buffer of arbitrary length over 100 to
	 * ensure pathlen isn't too long */
	char buf[MAX_PATH]; 
	if (S_ISLNK(sb->st_mode)) {
		/* read link from path into buf, throw error on failure */
		if (readlink(path, buf, MAX_PATH) == -1) {
			perror("readlink");
			exit(EXIT_FAILURE);
		}
		/* if strlen is greater than 100, can't fit into field */
		if (strlen(buf) > 100) {
			fprintf(stderr, "linkname too long");
			exit(EXIT_FAILURE);
		} else {
			/* else copy string to checksum field */
			strncpy(header->chksum, buf, MAX_NAME);
		}
	}
}

/* takes header struct and stat buffer,
 * populates uname and gname fields */
void pop_symnames(Header *header, struct stat *sb) {
	struct passwd *pw = getpwuid(sb->st_uid); /* contains user name */
	struct group *grp = getgrgid(sb->st_gid); /* contains group name */
	/* if getpwuid failed, throw an error */
	if (pw == NULL) {
		perror("getpwuid");
		exit(EXIT_FAILURE);
	}
	/* if getgrgid failed, throw an error */
	if (grp == NULL) {
		perror("getgrgid");
		exit(EXIT_FAILURE);
	}
	/* write the strings to their corresponding fields */
	strncpy(header->uname, pw->pw_name, sizeof(header->uname));
	strncpy(header->gname, grp->gr_name, sizeof(header->gname));
	return;
}

/* takes header, populates checksum field */
void pop_chksum(Header *header){
	/*sets chksum to spaces before hand for calculations*/
	memset(header->chksum,' ',sizeof(header->chksum));
	unsigned char *bytes = (unsigned char *) header;
	unsigned long checksum=0;
	size_t i;
	for(i=0;i<sizeof(Header);i++){
		checksum += bytes[i] ? 1 : 0;
	}
	/*writes the octal value of total bytes*/
	int_to_octal(header->chksum,sizeof(header->chksum),checksum);
	return;
}

/* takes header, struct stat, populates devmajor and devminor fields */
void pop_dev(Header *header, struct stat *sb) {
	unsigned int dev_maj; /* major number */
	unsigned int dev_min; /* minor number */
	/* if special file, write major and minor number as octal strings
	 * to their corresponding header fields */
	if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
		dev_maj = major(sb->st_mode);
		dev_min = minor(sb->st_mode);

        int_to_octal(header->devmajor,
                sizeof(header->devmajor), dev_maj);
        int_to_octal(header->devminor,
                sizeof(header->devminor), dev_min);
	}
	return;
}

void write_header(Header *header, char *path, int tarfile) {
	uint8_t buf[BLOCK_SIZE];
	int file, bytes_read, bytes_wrote, i;

	if (write(tarfile, header, BLOCK_SIZE) == -1) {
		perror("write");
		exit(EXIT_FAILURE);
	}
	if(header->typeflag[0] == '0'){
		if ((file = open(path, O_RDONLY)) == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}	
	
		while((bytes_read = read(file, buf, BLOCK_SIZE)) > 0) {
			if (bytes_read < BLOCK_SIZE) {
				for (i = bytes_read - 1; i < BLOCK_SIZE; i++) {
					buf[i] = '\0';
				}
			}
            if ((bytes_wrote = write(tarfile, buf, BLOCK_SIZE)) == -1) {
				perror("write");
				exit(EXIT_FAILURE);
			} 
		}
		close(file);
	}
	return;
}

void write_end(int tarfile) {
	char buf[BLOCK_SIZE * 2];
	memset(buf, '\0', BLOCK_SIZE * 2);
	if (write(tarfile, buf, BLOCK_SIZE * 2) == -1) {
		perror("write");
		exit(EXIT_FAILURE);
	}
}

