#include <stdio.h>
#include <stdlib.h>

/* these defines good practice? */
#define ON 1
#define OFF 0

/* should options be global?
 * other options? (list?) */
int c_option = OFF, t_option = OFF, x_option = OFF,
	v_option = OFF, f_option = OFF, S_option = OFF;

/* okay to leave functions like these in main? */
void check_options(char *options);

int main(int argc, char *argv[]) {
	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	check_options(argv[1]);
}

/* takes string of options, iterates through options and turns
 * on corresponding flags. returns void */
void check_options(char *options) {
	int i;
	
	/* pass every character in options string through switch case */
	for(i = 0; options[i] != 0; i++) {
		/* if character matches an option, turn on corresponding flag,
		 * else throw error */
		switch(options[i]) {
			case 'c':
				c_option = ON;
				break;
			case 't':
				t_option = ON;
				break;
			case 'x':
				x_option = ON;
				break;
			case 'v':
				v_option = ON;
				break;
			case 'f':
				f_option = ON;
				break;
			case 'S':
				S_option = ON;
				break;
			default:
				fprintf(stderr, "not a valid option: %c\n", options[i]);
				exit(EXIT_FAILURE);
		}
	}
	
	/* f option need to be at end of options? included in flag check? */	
	if (!f_option) {
		fprintf(stderr, "options must contain 'f'\n");
		exit(EXIT_FAILURE);
	}
	
	/* if c, t, or x flags are not present, throw error*/
	if(!(c_option || t_option || x_option)) {
		fprintf(stderr, "options must contain one of: 'c', 't', 'x'\n");
		exit(EXIT_FAILURE);
	}
}

