#include "tar_stuff.h"

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
	
	/* if more than one of c, t, or x flags, or if
	 * these flags not present, throw error */
	if((opts->c + opts->t + opts->x != 1)) {
		fprintf(stderr, "options must contain one and only one of: 'c', 't', 'x'\n");
		exit(EXIT_FAILURE);
	}
    return opts;
}

/* function to store octal string of val into dest */
void int_to_octal(char *dest,int size, unsigned long val){
	snprintf(dest, size, "%0*lo", size-1, val);
}

