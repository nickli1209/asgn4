#include "tar_stuff.h"

// when verbose is not set, read each header section at name_offset and print the file name
// when verbose is set, read each header at name_offset for name, mode_offset for permissions and convert to
// drwx---rw- format, and so on for the rquired feilds, then pad with spaces to the proper length of each feild and print
//

void contents(int tarfile) {
  char name[MAX_NAME];
  char prefix[MAX_PREFIX + 1]; /* to put slash */
  char fullpath[MAX_PATH];
  char octal_size[MAX_SIZE];
  unsigned long size;
  int offset;

  do {
  if (read(tarfile, name, MAX_NAME) == -1) {
    perror("read name");
    exit(EXIT_FAILURE);
  }

  lseek(tarfile, 24, SEEK_CUR);
  if (read(tarfile, octal_size, MAX_SIZE) == -1) {
    perror("read size");
    exit(EXIT_FAILURE);
  }
  printf("%s\n", octal_size);
  size = strtoul(octal_size, NULL, 8);

  lseek(tarfile, 209, SEEK_CUR);
  if (read(tarfile, prefix, MAX_PREFIX) == -1) {
    perror("read prefix");
    exit(EXIT_FAILURE);
  }
  if (prefix[0] != '\0') {
    strcat(prefix, "/");
    strcpy(fullpath, prefix);
    strcat(fullpath, name);
  } else {
    strcpy(fullpath, name);
  }
  printf("%s\n", fullpath);
  lseek(tarfile, 12, SEEK_CUR);

  offset = (size / 512) + 1;
  lseek(tarfile, offset * 512, SEEK_CUR);
  } while (name[0] != '\0');

  return;
}
