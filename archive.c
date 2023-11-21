#include "tar_stuff.h"

void traverse_files(char *path, Options *opts) {
	DIR *dir;
	struct dirent *ent;
	struct stat sb;
	char fullpath[PATH_MAX];
	
	if (opts->v) {
		printf("%s/\n", path);
	}

	/* open current directory */
	if ((dir = opendir(path)) == NULL) {
		perror("opendir");
		exit(EXIT_FAILURE); //prob don't wan't to exit here.
	}
	
	/* iterate through current's entries */
	/* what do I do if I can't read an entry? */
	while ((ent = readdir(dir)) != NULL) {
		/* skip over '.' and '..' entries  */
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				snprintf(fullpath, PATH_MAX, "%s/%s", path, ent->d_name);

				if (lstat(fullpath, &sb) == -1) {
					perror("lstat");
					closedir(dir);
					exit(EXIT_FAILURE); //prob shouldn't exit here
				}

				if (S_ISDIR(sb.st_mode) && !S_ISLNK(sb.st_mode)) {
					traverse_files(fullpath, opts);
				} else {
					if (opts->v) {
						printf("%s\n", fullpath);
					}
				}
			}
	}
	closedir(dir);
}
