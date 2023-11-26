#include "tar_stuff.h"

int main(int argc, char *argv[]) {
	Options *opts;
	char *path;
	int pathlen, i, tarfile;

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
		if ((tarfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 
			S_IRWXU | S_IRWXG)) == -1) {
			perror("open");
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
			traverse_files(path, opts, tarfile);
		}
		write_end(tarfile);
        close(tarfile);     
	}	

	if (opts->t) {
		if ((tarfile = open(argv[2], O_RDONLY)) == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}
		contents(tarfile, opts);
		close(tarfile);
	}
	return 0;
}
