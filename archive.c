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
