#include "tar_stuff.h"

// when verbose is not set, read each header section at name_offset and print the file name
// when verbose is set, read each header at name_offset for name, mode_offset for permissions and convert to
// drwx---rw- format, and so on for the rquired feilds, then pad with spaces to the proper length of each feild and print
//

void contents(int tarfile) {
  char name[MAX_NAME];
  char prefix[MAX_PREFIX];

  lseek(tarfile, 0, SEEK_SET);
  if (read(tarfile, name, MAX_NAME) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  printf("%s\n", name);
  return;
}
