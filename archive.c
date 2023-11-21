#include "tar_stuff.h"

void traverse_files(char *path, Options *opts) {
	DIR *dir; /* current directory */
	struct dirent *ent; /* entries inside dir */
	struct stat sb; /* stat buffer for entries */
	char fullpath[PATH_MAX]; /* holds full path */
	
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
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				/* format new path, store in fullpath */
				snprintf(fullpath, PATH_MAX, "%s/%s", path, ent->d_name);

				/* store entry info into sb */
				if (lstat(fullpath, &sb) == -1) {
					perror("lstat");
					closedir(dir);
					exit(EXIT_FAILURE); /* prob shouldn't exit here */
				}

				/* if it's a directory, recurse */
				if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
					traverse_files(fullpath, opts);
				} else {
					/* else print path if verbose */
					if (opts->v) {
						printf("%s\n", fullpath);
					}
				}
			}
	}
	closedir(dir);
}

// after calling, the prebiously defined header struct will be populated with
// all necessary info... proceed to write header after
void fill_header(Header * header, char file_path){
	memset(header,'\0',sizeof(Heasder));
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

































