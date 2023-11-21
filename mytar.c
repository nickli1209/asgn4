#include "tar_stuff.h"

int main(int argc, char *argv[]) {
	Node *head = NULL;
	Options *opts;
	char *path;
	int pathlen, i;

	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	/* store options in Options struct  */
	opts = check_options(argv[1]);
	
	/* create */
	if (opts->c) {
		if (argc < 4) {
			fprintf(stderr, "usage: mytar [txvS]cf tarfile [ path [ ... ] ]\n");
			exit(EXIT_FAILURE);
		}
		if ((path = malloc(255)) == NULL) {
			perror("malloc on path");
			exit(EXIT_FAILURE);	
		}
		/* traverse every file in path argument */
		for (i = 3; i < argc; i++) {
			if (strlen(argv[i]) > MAX_PATH) {
				fprintf(stderr, "pathname over 256 characters");
				exit(EXIT_FAILURE);
			}
			pathlen = strlen(argv[i]) + 1;
			memmove(path, argv[i], pathlen);
			head = traverse_files(head, path, opts);
		}	
	}	

	return 0;
}
