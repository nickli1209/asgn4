#include "tar_stuff.h"

/* takes head of linked list, path string, and options,
 * does a preorder DFS of file tree. returns head of linked
 * list of Header structs */
Node *traverse_files(Node *head, char *path, Options *opts) {
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
	
	header = create_header(path, &sb, opts);
	head = insert_end(head, header);

	/* if verbose option on, print current directory path*/
	if (opts->v) {
		printf("%s/\n", path);
	}

	/* open current directory */
	if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(EXIT_FAILURE); /*prob don't wan't to exit here.*/
	}
	
	/* iterate through current's entries */
	while ((ent = readdir(dir)) != NULL) { 	/* what do I do if I can't read an entry? */
		/* skip over '.' and '..' entries  */
		if (strcmp(ent->d_name, ".") != 0 && 
		strcmp(ent->d_name, "..") != 0) {
				/* check to make sure path isn't too long before creating it*/
				if (strlen(path) + strlen(ent->d_name) > MAX_PATH) {
					fprintf(stderr, "pathname over 256 characters");
					exit(EXIT_FAILURE);
				}
				/* format new path, store in fullpath */
				snprintf(fullpath, MAX_PATH, "%s/%s", path, ent->d_name);

				/* store entry info into sb */
				if (lstat(fullpath, &sb) == -1) {
					perror("lstat");
					closedir(dir);
					exit(EXIT_FAILURE); /* prob shouldn't exit here */
				}

				/* if it's a directory, recurse */
				if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
					head = traverse_files(head, fullpath, opts);
				} else {
					/* else print path if verbose */
					header = create_header(fullpath, &sb, opts);
					head = insert_end(head, header);
					if (opts->v) {
						printf("%s\n", fullpath);
					}
				}
			}
	}
	closedir(dir);
	return head;
}

/* takes head Node and Header structs, creates Node out 
 * of Header and inserts it to end of linked list. returns head*/
Node *insert_end(Node *head, Header *header) {
  Node *new; /* Node being inserted */
	Node *cur; /* used for traversing to end */

	/* malloc space for new Node */
	if ((new = malloc(sizeof(Node))) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	/* populate new Node with Header's info */
  new->header = header;
  new->next = NULL;

	/* if list is empty, assign head to new Node */
  if (head == NULL) {
    head = new;
  } else {
		/* else iterate from head to end, set end to new */
    cur = head;
    while (cur->next != NULL) {
      cur = cur->next;
    }
    cur->next = new;
  }
  return head;
}

/* TODO */
Header *create_header(char *name, struct stat *sb, Options *opts) {
	Header *header;
	if ((header = malloc(sizeof(Header))) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* helps w/ NULL terminating */
	memset(header, '\0', sizeof(Header));

	pop_name(header, name); /* name */
	int_to_octal(header->mode, sizeof(header->mode), sb->st_mode); /* mode */
	pop_IDs(header, sb, opts); /* uid and gid */

	/* size */
	if (!S_ISLNK(sb->st_mode) && !S_ISDIR(sb->st_mode)) {
		int_to_octal(header->size, sizeof(header->size), sb->st_size);
	} else {
		int_to_octal(header->size, sizeof(header->size), 0);
	}

	/* mtime */
	int_to_octal(header->mtime, sizeof(header->mtime), (unsigned int)sb->st_mtime);
	pop_typeflag(header, sb); /* typeflag */
	pop_linkname(header, name, sb); /* linkname */
	strcpy(header->magic, "ustar"); /* magic */
	strncpy(header->version, "00", 2); /* version (NOT NULL terminated) */
	pop_symnames(header, sb); /* uname and gname */
	pop_chksum(header); /* checksum */
	return header;
}

/* called in create_header. Takes Header and pathname string,
 * populates header name and prefix with pathname */
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
    strncpy(header->prefix, fullpath, index-1); /* -1 since we don't need the slash */
  }
  return;
}

/* populates header with user/group IDs */
void pop_IDs(Header *header, struct stat *sb, Options *opts) {
	if (sb->st_uid <= 07777777) {
		/* if the st_uid can fit into 7 octal digits, create octal string and 
		 * throw it in the header */
		int_to_octal(header->uid, sizeof(header->uid), sb->st_uid);
	} else if (!opts->S) {
		/* else if S option not ON, use insert_special_int */
		insert_special_int(header->uid, sizeof(header->uid), sb->st_uid);
	} else {
		/* otherwise, throw error */
		fprintf(stderr, "user ID does not fit in header: turn off S option\n");
		exit(EXIT_FAILURE);
	}
	/* likewise for group IDs */
	if (sb->st_gid <= 07777777) {
		int_to_octal(header->gid, sizeof(header->gid), sb->st_gid);
	} else if (!opts->S) {
		insert_special_int(header->gid, sizeof(header->gid), sb->st_gid);
	} else {
		fprintf(stderr, "group ID does not fit in header: turn off S option\n");
		exit(EXIT_FAILURE);
	}
	return;
}

void pop_typeflag(Header *header, struct stat *sb) {
	if (S_ISREG(sb->st_mode)) {
		header->typeflag = '0';
	} else if (S_ISLNK(sb->st_mode)) {
		header->typeflag = '2';
	} else if (S_ISDIR(sb->st_mode)) {
		header->typeflag = '5';
	} else {
		header->typeflag = '\0';
	}
	return;
}

void pop_linkname(Header *header, char *path, struct stat *sb) {
	char buf[MAX_PATH]; /* arbitrary length over 100 to ensure pathlen isn't too long */
	if (S_ISLNK(sb->st_mode)) {
		if (readlink(path, buf, MAX_PATH) == -1) {
			perror("readlink");
			exit(EXIT_FAILURE);
		}
		if (strlen(buf) > 100) {
			fprintf(stderr, "linkname too long");
			exit(EXIT_FAILURE);
		} else {
			strncpy(header->chksum, buf, MAX_NAME);
		}
	}
}

void pop_symnames(Header *header, struct stat *sb) {
	struct passwd *pw = getpwuid(sb->st_uid);
	struct group *grp = getgrgid(sb->st_gid);

	if (pw == NULL) {
		perror("getpwuid");
		exit(EXIT_FAILURE);
	}
	if (grp == NULL) {
		perror("getgrgid");
		exit(EXIT_FAILURE);
	}

	strncpy(header->uname, pw->pw_name, sizeof(header->uname));
	strncpy(header->gname, grp->gr_name, sizeof(header->gname));
	return;
}

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
}

































