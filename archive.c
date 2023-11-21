#include "tar_stuff.h"
/*
void process_roots(char *name, char*name, Options *opts) {
	DIR *dir;
	if ((dir = openddir(".")	

	
	if (chdir(name) == -1) {
		perror("chdir");
		exit(EXIT_FAILURE);
	}
	


}
*/

void traverse_files(char *path, char *name, Options *opts) {
	DIR *cur_dir;
	struct dirent *cur_ent;
	struct stat sb;
	char cur_path[PATH_MAX];
	char cwd[PATH_MAX*2];
	
	/* change dir to relative path (name)  */
	if (chdir(name) == -1) {
		perror("chdir");
		exit(EXIT_FAILURE);
	}

	/* open current directory */
	if ((cur_dir = opendir(".")) == NULL) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	
	/* iterate through current's entries */
	/* what do I do if I can't read an entry? */
	while ((cur_ent = readdir(cur_dir)) != NULL) {
		/* skip over '.' and '..' entries  */
		if (strcmp(cur_ent->d_name, ".") != 0 && strcmp(cur_ent->d_name, "..") != 0) {
			if (lstat(cur_ent->d_name, &sb) == -1) {
				perror("lstat");
				exit(EXIT_FAILURE);
			}
			
			/* add current entry name to path */
			strcpy(cur_path, path);
			strcat(cur_path, "/");
			strcat(cur_path, cur_ent->d_name);
	
			/* if verbose option on, print paths as we go */
			if (opts->v){
				printf("%s\n", cur_path);
			}

			/* if entry is directory, not symlink */
			if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
				/* save current working directory before recursing  */
				if (getcwd(cwd, PATH_MAX*2) == NULL) {
					perror("cwd");
					exit(EXIT_FAILURE);
				}
				
				/* call self on directory */	
				traverse_files(cur_path, cur_ent->d_name, opts);
				
				/* change back to parent */
				if (chdir(cwd) == -1) {
					perror("chdir to cwd");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	closedir(cur_dir);
	/* will this chdir back to root? */
}
