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

	pop_name(header, name); /* name */
	int_to_octal(header->mode, sizeof(header->mode), sb->st_mode); /* mode */
	pop_IDs(header, sb, opts); /* uid and gid */
	/* size */
	if (!S_ISLNK(sb->st_mode) && !S_ISDIR(sb->st_mode)) {
		int_to_octal(header->size, sizeof(header->size), sb->st_size);
	} else {
		int_to_octal(header->size, sizeof(header->size), 0);
	}

	/* what the fuck - mtime */
	// printf("real mtime: %o\n", sb->mtim);
	// int_to_octal(header->mtime, sizeof(header->mtime), sb->st_mtim);
	// printf("header mtime: %s\n", header->mtime);

	return header;
}

/* called in create_header. Takes Header and pathname string,
 * populates header name and prefix with pathname */
void pop_name(Header *header, char *fullpath) {
  unsigned int index; /* current index, used if prefix is needed */

	/* memset both fields to NULL char so if there is space
	 * for one we do not have to add it later */
	/* maybe redundant? */
  memset(header->prefix, '\0', MAX_PREFIX); 
  memset(header->name, '\0', MAX_NAME);

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

/* populates header user/group IDs - if S option,
 *  use insert_special_int function */
void pop_IDs(Header *header, struct stat *sb, Options *opts) {
	if (opts->S) {
		insert_special_int(header->uid, sizeof(header->uid), sb->st_uid);
		insert_special_int(header->gid, sizeof(header->gid), sb->st_gid);
	} else {
		int_to_octal(header->uid, sizeof(header->uid), sb->st_uid);
		int_to_octal(header->gid, sizeof(header->gid), sb->st_gid);
	}
	return;
}

/*
// after calling, the prebiously defined header struct will be populated with
// all necessary info... proceed to write header after
void fill_header(Header * header, char file_path){
	memset(header,'\0',sizeof(Header));
	struct stat filestats;
	if(lstat(".",&filestats) !=0){
        perror("Error getting stat of path in fill_header()");
        exit(1);
        }
	//fill the header structs fields with data from stat, need to build
	//a function to take destination of header->mode etc and source of
	//stat.st_mode etc and convert the int held by st_mode to octal
	

	//takes care of splitting path between name and prefix
	//logic may be a touch wrong teehee
	size_t path_len = strlen(path);
	if(path_len<=100){
		strncpy(header->name,file_path,100);
	}else{
		char *slash =file_path + path_len-100;
		while(slash>file_path && *slash !='/'){
			slash--;
		}	
		if (slash==file_path){
			perror("no slash was found between name and the end of prefix");
			exit(EXIT_FAILURE);
		}
		size_t prefix_len=slash-file_path;
		if (prefix_len>155){
			perror("path name too long to fit in prefix and name");
			exit(EXIT_FAILURE);
		}
		strncpy(header->prefix,path,prefix_len);
		strncpy(header->name,slash+1,100);
	}
	//----------------------------------
	int_to_octal(header->mode,sizeof(header->mode),filestats.st_mode & 07777);
	int_to_octal(header->uid,sizeof(header->uid),filestats.st_uid);
	int_to_octal(header->gid,sizeof(header->gd),filestats.st_gid);
	if(S_ISREG(filestats.st_mode)){
		int_to_octal(header->size,sizeof(header->size),filestast.st_size);
	}else{	
		int_to_octal(header->size,sizeof(header->size),0);
	}
	int_to_octal(header->mtime,sizeof(header->mtime),filesstats.st_mtime);
	struct passwd *pw=getpwuid(filestats.st_uid);
	if(pw){
		strncpy(header->uname,pw->pw_name,sizeof(header->uname));
	}else{
		perror("error getting psswd struct in fill_header");
		exit(EXIT_FAILURE);
	}
	struct group *group=getgrgid(filestats.st_gid);
	if(group){
		strncpy(header->gname,group->gr_name,sizeof(header->gname));
	}else{
		perror("error getting group struct in fill header");
		exit(EXIT_FAILURE);
	}
	//setting typeflag and linkname
	if(S_ISREG(filestats.st_mode)){
		header->typeflag='0';
		header->linkname[0]='\0';
	}else if(S_ISDIR(filestats.st_mode)){
		header->typeflag='5';
		header->linkname[0]='\0';
	}else if(S_ISLINK(filestats.st_mode)){
		header->typeflag='2';
		ssize_t link_len =readlink(file_path,header->linkname,sizeof(header->linkname));
		if(link_len<0){
			perror("error performing readlink in fill_header");
			exit(EXIT_FAILURE);
		}else if(link_len>100){
			fprintf(stderr,"error linkname too long");
			exit(EXIT_FAILURE);
		}
		else{
			header->linkname[link_len]='\0';
		}
	}else{
		header->typeflag='\0';
		header->linkname[0]='\0';
	}
	//------------------------------------
	// still need to populate magic, version,chksum,devmajor,devminor
	
}
*/

































