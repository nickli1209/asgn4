#include "tar_stuff.h"

/* these defines good practice? */
#define ON 1
#define OFF 0

typedef struct{
    int c,t,x,v,f,S;
}Options;

/* okay to leave functions like these in main? */
Options *check_options(char *options);

int main(int argc, char *argv[]) {
	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	Options *opts = check_options(argv[1]);
	printf("c: %d", opts->c);
	return 0;
}

/* takes string of options, iterates through options and turns
 * on corresponding flags. returns void */
Options *check_options(char *options) {
	Options *opts = malloc(sizeof(Options));
	int i;
	/* pass every character in options string through switch case */
	for(i = 0; options[i] != '\0'; i++) {
		/* if character matches an option, turn on corresponding flag,
		 * else throw error */
		switch(options[i]) {
			case 'c':
				opts->c = ON;
				break;
			case 't':
				opts->t = ON;
				break;
			case 'x':
				opts->x = ON;
				break;
			case 'v':
				opts->v = ON;
				break;
			case 'f':
				opts->f = ON;
				break;
			case 'S':
				opts->S = ON;
				break;
			default:
				fprintf(stderr, "not a valid option: %c\n", options[i]);
				exit(EXIT_FAILURE);
		}
	}

	/* check to make sure f option exists */	
	if (!opts->f) {
		fprintf(stderr, "options must contain 'f'\n");
		exit(EXIT_FAILURE);
	}
	
	/* c, t, or x flags are not present, throw error */
	if(!(opts->c || opts->t || opts->x) && !(opts->c && opts->t) && !(opts->c && opts->x) && !(opts->t && opts->x)) {
		fprintf(stderr, "options must contain one and only one of: 'c', 't', 'x'\n");
		exit(EXIT_FAILURE);
	}
    return opts;
}

