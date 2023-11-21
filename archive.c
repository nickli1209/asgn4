#include "tar_stuff.h"

Node *traverse_files(Node *head, char *path, Options *opts) {
	DIR *dir; /* current directory */
	struct dirent *ent; /* entries inside dir */
	struct stat sb; /* stat buffer for entries */
	char fullpath[MAX_PATH]; /* holds full path */
	Header *header;
	
	
	header = pop_header(path);
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
					header = pop_header(fullpath);
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

Node *insert_end(Node *head, Header *header) {
  Node *new = malloc(sizeof(Node));
	Node *cur;
  new->header = header;
  new->next = NULL;

  if (head == NULL) {
    head = new;
  } else {
    cur = head;
    while (cur->next != NULL) {
      cur = cur->next;
    }
    cur->next = new;
  }
  return head;
}

Header *pop_header(char *name) {
	Header *header;
	if ((header = malloc(sizeof(Header))) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	header = pop_name(header, name);
	return header;
}

Header *pop_name(Header *header, char *fullpath) {
  unsigned int index;
  char prefix[155];
  char name[100];

  memset(prefix, '\0', 155);
  memset(name, '\0', 100);

  if (strlen(fullpath) <= 100) {
    strncpy(name, fullpath, strlen(fullpath));
  } else {
    index = strlen(fullpath) - 100;
    while (fullpath[index] != '/') {
      if (index >= strlen(fullpath)-1) {
        fprintf(stderr, "pathname too long");
        exit(EXIT_FAILURE);
      }
      index++;
    }
    index++;
    strncpy(name, &fullpath[index], strlen(fullpath) - index);
    strncpy(prefix, fullpath, index-1);
  }
  strcpy(header->name, name);
  strcpy(header->prefix, prefix);
  return header;
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

































