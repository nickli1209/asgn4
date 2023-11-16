#include "tar_stuff.h"

int main(int argc, char *argv[]) {
	Options *opts;
	char *path;
	int pathlen, i;

	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}

	opts = check_options(argv[1]);

	if (opts->c) {
		if (argc < 4) {
			fprintf(stderr, "usage: mytar [txvS]cf tarfile [ path [ ... ] ]\n");
			exit(EXIT_FAILURE);
		}
		if ((path = malloc(255)) == NULL) {
			perror("malloc on path");
			exit(EXIT_FAILURE);	
		}
		
		for (i = 3; i < argc; i++) {
			pathlen = strlen(argv[i]);
			memmove(path, argv[i], pathlen);
			path[pathlen] = '\0';
			traverse_files(path, pathlen, opts);
		}	


	}	

	return 0;
}
