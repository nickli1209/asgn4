#include "tar_stuff.h"

int main(int argc, char *argv[]) {
	Options *opts;

	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	opts = check_options(argv[1]);
	printf("c: %d", opts->c);
	return 0;
}
