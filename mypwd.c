#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void write_path(char *path, int path_index);

int main(int argc, char *argv[]) {
	/* malloc string for path of PATH_MAX */
    char *path = malloc(PATH_MAX);
    if (path == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    int path_index = PATH_MAX - 1; /* current index in path */
    path[path_index--] = '\0';

	/* write the entire path */
    write_path(path, path_index);
    free(path);

    return 0;
}

/* takes path string and index in path,
 * writes path backwards and prints to stdout,
 * returns void  */
void write_path(char *path, int path_index) {
    DIR *d; /* current directory */
    struct stat cur_sb; /* buffer for current entry */
    struct stat root_sb; /* buffer for root */
    struct stat check; /* used to check against current */
    struct dirent *current_entry; /* directory entries */
    
	/* stat the root so we can
	 * compare in the while loop  */
    if (lstat("/", &root_sb) == -1) {
        perror("mypwd");
        exit(EXIT_FAILURE);
    }

    do {
        /* stat the current directory  */
        if (lstat(".", &cur_sb) == -1) {
            perror("mypwd");
            exit(EXIT_FAILURE);
        }

        /* change directory to parent  */
        if (chdir("..") == -1) {
            perror("mypwd");
            exit(EXIT_FAILURE);
        }

        /* open current directory  */
        if ((d = opendir(".")) == NULL) {
            perror("cannot get current directory");
            exit(EXIT_FAILURE);
        }
        
        /* stat every link looking for one
		 * that goes to where we came from (cur_sb)  */
        while ((current_entry = readdir(d)) != NULL) { 
            /* stat our current entry */
            if (lstat(current_entry->d_name, &check) == -1) {
                perror("mypwd-curentry check");
				fprintf(stderr, "Failed to lstat entry: %s\n",
					   	current_entry->d_name);
                exit(EXIT_FAILURE);
            }
			/* if inodes and devie numbers match,
			 * we have found the right file  */
			if (cur_sb.st_ino == check.st_ino &&
				   	cur_sb.st_dev == check.st_dev) {
				/* skip over '.' and '..' entries  */
				if (strcmp(current_entry->d_name, ".") != 0 &&
					strcmp(current_entry->d_name,
					"..") != 0) {
					/* get length of string
					 * to find correct index */
                    int len = strlen(current_entry->d_name);
                    if (path_index - len < 0) {
                        fprintf(stderr, "path too long");
                        exit(EXIT_FAILURE);
                    }
					/* write path name to our path  */
                    path_index -= len;
                    memmove(&path[path_index], current_entry->d_name, len);
                    path[--path_index] = '/';
					/* get out of while loop once we find 
					 * the right file  */	
                    break;
                }
            }
        }
		closedir(d);
    } while ((root_sb.st_ino != cur_sb.st_ino) &&
		   	(root_sb.st_dev != cur_sb.st_dev));
	/* print path, change directories back to path */
    printf("%s\n", &path[path_index]);
    if (chdir(&path[path_index]) == -1) {
        perror("mypwd");
        exit(EXIT_FAILURE);
    }
    return;
} 
