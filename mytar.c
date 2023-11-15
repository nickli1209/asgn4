#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
}
