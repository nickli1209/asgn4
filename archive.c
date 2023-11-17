#include "tar_stuff.h"

void traverse_files(char *path, char *name, int path_index, Options *options) {
	DIR *cur_dir;
	struct dirent *cur_ent;
	struct stat sb;
	char cwd[PATH_MAX*2];
	
	if (chdir(name) == -1) {
		perror("chdir");
		exit(EXIT_FAILURE);
	}	
	/* don't exit? */
	if ((cur_dir = opendir(".")) == NULL) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	
	
	while ((cur_ent = readdir(cur_dir)) != NULL) {
		if (strcmp(cur_ent->d_name, ".") != 0 && strcmp(cur_ent->d_name, "..") != 0) {
			if (lstat(cur_ent->d_name, &sb) == -1) {
				perror("lstat");
				exit(EXIT_FAILURE);
			}
			printf("path: %s\n", path);
			printf("file: %s\n", cur_ent->d_name);
			printf("cwd: %s\n", cwd);
			path[path_index++] = '/';
			strcpy(&path[path_index], cur_ent->d_name);
			path_index += strlen(cur_ent->d_name);

			if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
				if (getcwd(cwd, PATH_MAX*2) == NULL) {
					perror("cwd");
					exit(EXIT_FAILURE);
				}
				
				/*strcpy(name, cur_ent->d_name);*/	
				
				traverse_files(path, cur_ent->d_name, path_index, options);
				
				if (chdir(cwd) == -1) {
					perror("chdir to cwd");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	closedir(cur_dir);
}
