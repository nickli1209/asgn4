#include "tar_stuff.h"


/* should options be global?
 * other options? (list?) */


int main(int argc, char *argv[]) {
	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	Options opts = check_options(argv[1]);
	return 0;
}

