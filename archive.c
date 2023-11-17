#include "tar_stuff.h"

void traverse_files(char *path, char *name, int path_index, Options *options) {
	DIR *cur_dir;
	struct dirent *cur_ent;
	struct stat sb;
	char cwd[PATH_MAX*2];
	char cur_path[PATH_MAX];	

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

			strcpy(cur_path, path);
			strcat(cur_path, "/");
			strcat(cur_path, cur_ent->d_name);

			printf("path: %s\n", cur_path);
			printf("file: %s\n", cur_ent->d_name);
			printf("cwd: %s\n", cwd);

			/*
			path[path_index++] = '/';
			strcpy(&path[path_index], cur_ent->d_name);
			*/
			path_index += strlen(cur_ent->d_name);

			if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
				if (getcwd(cwd, PATH_MAX*2) == NULL) {
					perror("cwd");
					exit(EXIT_FAILURE);
				}
				
				/*strcpy(name, cur_ent->d_name);*/	
				
				traverse_files(cur_path, cur_ent->d_name, path_index, options);
				
				if (chdir(cwd) == -1) {
					perror("chdir to cwd");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	closedir(cur_dir);
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
	


	//handle the differences between strict and non strict, magic, version
	// set the type flag by checking if reg or directory etc
	

	
}
