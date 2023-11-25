#include "tar_stuff.h"

// when verbose is not set, read each header section at name_offset and print the file name
// when verbose is set, read each header at name_offset for name, mode_offset for permissions and convert to
// drwx---rw- format, and so on for the rquired feilds, then pad with spaces to the proper length of each feild and print
//

/* TODO make this a function in tar_stuff.c - useful for reading headers
 * should read header one at a time and return it. This might be were a linked 
 * list is useful */
void contents(int tarfile) {
  uint8_t buf[BLOCK_SIZE];
  char fullpath[MAX_PATH];
  unsigned long size;
  int offset;
  Header *header;
  uint8_t check[BLOCK_SIZE];

  memset(check, '\0', BLOCK_SIZE);

  while (read(tarfile, buf, BLOCK_SIZE) != -1) {
    if (memcmp(buf, check, BLOCK_SIZE) == 0) {
      break;
    }
    /* malloc space for Header struct */
    if ((header = malloc(sizeof(Header))) == NULL) {
      perror("malloc on Header struct");
      exit(EXIT_FAILURE);
    }

    memcpy(header->name, &buf[NAME_OFFSET], MAX_NAME); /* read name into */
    memcpy(header->mode, &buf[MODE_OFFSET], MAX_MODE);
    memcpy(header->uid, &buf[UID_OFFSET], MAX_ID);
    memcpy(header->gid, &buf[GID_OFFSET], MAX_ID);
    memcpy(header->size, &buf[SIZE_OFFSET], MAX_SIZE); /* copy size to string */
    size = strtoul(header->size, NULL, 8); /* convert it to an integer */
    memcpy(header->mtime, &buf[MTIME_OFFSET], MAX_MTIME);
    memcpy(header->chksum, &buf[CHKSUM_OFFSET], MAX_CHKSUM);
    memcpy(header->typeflag, &buf[TYPEFLAG_OFFSET], MAX_TYPEFLAG);
    memcpy(header->linkname, &buf[LINKNAME_OFFSET], MAX_LINKNAME);
    memcpy(header->magic, &buf[MAGIC_OFFSET], MAX_MAGIC);
    memcpy(header->version, &buf[VERSION_OFFSET], MAX_VERSION);
    memcpy(header->uname, &buf[UNAME_OFFSET], MAX_UNAME);
    memcpy(header->gname, &buf[GNAME_OFFSET], MAX_GNAME);
    memcpy(header->devmajor, &buf[DEVMAJ_OFFSET], MAX_DEVMAJ);
    memcpy(header->devminor, &buf[DEVMIN_OFFSET], MAX_DEVMIN);
    memcpy(header->prefix, &buf[PREFIX_OFFSET], MAX_PREFIX);

    if (header->prefix[0] != '\0') {
      snprintf(fullpath, MAX_PATH, "%s/%s", header->prefix, header->name);
    } else {
      strncpy(fullpath, header->name, MAX_NAME);
    }
    printf("%s\n", fullpath);

    offset = size ? ((size / 512) + 1) : 0; /* calculate block to read */
    lseek(tarfile, offset * 512, SEEK_CUR); /* seek to next header block */
  }

  close(tarfile);
  return;
}
