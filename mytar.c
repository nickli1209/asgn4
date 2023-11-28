#include "tar_stuff.h"

int main(int argc, char *argv[]) {
	Options *opts = NULL;
	char *path = NULL;
	int pathlen, i, tarfile;

	/* check for at least 3 args */
	if (argc < 3) {
		fprintf(stderr,
                "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	/* store options in Options struct  */
    if ((opts = malloc(sizeof(Options))) == NULL) {
        perror("malloc on Options");
        exit(EXIT_FAILURE);
    }
    
	check_options(argv[1], opts);

	/* create */
	if (opts->c) {
        /* if there aren't enough args */
		if (argc < 4) {
			fprintf(stderr,
                    "usage: mytar [txvS]cf tarfile [ path [ ... ] ]\n");
			exit(EXIT_FAILURE);
		}
        
        /* malloc space for path arg */
		if ((path = malloc(255)) == NULL) {
			perror("failed to malloc path");
			exit(EXIT_FAILURE);	
		}

        /* open the tarfile arg, save its file descriptor */
		if ((tarfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 
			S_IRWXU | S_IRWXG)) == -1) {
			perror("failed to open tarfile");
			exit(EXIT_FAILURE);
		}

		/* traverse every file in path argument */
		for (i = 3; i < argc; i++) {
            /* if initial path is too big, skip to next one */
			if (strlen(argv[i]) > MAX_PATH) {
				printf("pathname over 256 characters: %s\n",
                        argv[i]);
				continue;
			}
            /* save path and begin write with it */
			pathlen = strlen(argv[i]) + 1;
			memmove(path, argv[i], pathlen);
			traverse_files(path, opts, tarfile);
		}
		write_end(tarfile); /* write last 2 blocks of NULLS */
        free(path);
        close(tarfile); /* close tarfile */     
	}	

    /* table of contents */
	if (opts->t) {
        int num_paths = 0; /* number of optional paths */
        char **paths = NULL; /* array of path strings */

        /* if there are optional paths */
        if (argc > 3) {
            num_paths = argc - 3;
            if ((paths = malloc(num_paths + 1 * sizeof(char *))) == NULL) {
                perror("failed to malloc paths in list");
                exit(EXIT_FAILURE);
            }
            /* fill paths array */
            for (i = 3; i < argc; i++) {
                paths[i - 3] = argv[i];
            }
            paths[++i] = NULL;
        }
        
        /* open tarfile, save its file descriptor */
		if ((tarfile = open(argv[2], O_RDONLY)) == -1) {
			perror("failed to open tarfile");
			exit(EXIT_FAILURE);
		}
        
        /* read, print contents of tarfile */
		contents(tarfile, opts, paths);
        free(paths); /* free paths */
		close(tarfile); /* close tarfile */
	}

    /* extract */
    if(opts->x) {
	int num_paths;
	char **paths=NULL;
	/*if optional paths*/
	if (argc > 3) {
            num_paths = argc - 3;
            if ((paths = malloc(num_paths + 1 * sizeof(char *))) == NULL) {
                perror("failed to malloc paths in extract");
                exit(EXIT_FAILURE);
            }
            /* fill paths array */
            for (i = 3; i < argc; i++) {
                paths[i - 3] = argv[i];
            }
            paths[++i] = NULL;
        }
	/*open set tarfile*/	
        if ((tarfile = open(argv[2], O_RDONLY)) == -1) {
            perror("failed to open tarfile");
            exit(EXIT_FAILURE);
        }
        extract_files(tarfile, opts,paths);
	free(paths);
	close(tarfile);
    }
    free(opts);
	return 0;
}
