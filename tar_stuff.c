#include "tar_stuff.h"

Options check_options(char *options) {
        Options opts ={0};
        int i;
        /* pass every character in options string through switch case */
        for(i = 0; options[i] != '\0'; i++) {
                /* if character matches an option, turn on corresponding flag,
                   else throw error */
                switch(options[i]) {
                        case 'c':
                                opts.c = ON;
                                break;
                        case 't':
                                opts.t = ON;
                                break;
                        case 'x':
                                opts.x = ON;
                                break;
                        case 'v':
                                opts.v = ON;
                                break;
                        case 'f':
                                opts.f = ON;
                                break;
                        case 'S':
                                opts.S = ON;
                                break;
                        default:
                                fprintf(stderr, "not a valid option: %c\n", options[i]);
                                exit(EXIT_FAILURE);
                }
        }

        /* f option need to be at end of options? included in flag check? */
        if (!opts.f) {
                fprintf(stderr, "options must contain 'f'\n");
                exit(EXIT_FAILURE);
        }

        /* if c, t, or x flags are not present, throw error*/
        if(!(opts.c || opts.t || opts.x)) {
                fprintf(stderr, "options must contain one of: 'c', 't', 'x'\n");
                exit(EXIT_FAILURE);
        }
    return opts;
}
