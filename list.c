#include "tar_stuff.h"

void contents(int tarfile, Options *opts) {
  uint8_t buf[BLOCK_SIZE];
  char fullpath[MAX_PATH];
  unsigned long size;
  int offset;
  Header *header;
  uint8_t check[BLOCK_SIZE];

  memset(check, '\0', BLOCK_SIZE);

  while (read(tarfile, buf, BLOCK_SIZE) > 0) {
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
    size = strtoul(header->size, NULL, 8); /* convert size to an integer */
    
    if (opts->v) {
      print_perms(header->mode);
      printf(" %s/%s        ", header->uname, header->gname);
      printf("%8d ", (int) size);
      time_t time = (time_t) strtoul(header->mtime, NULL, 8);
      struct tm *timeinfo = localtime(&time);
      char formatted_time[17];
      strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M", timeinfo);
      printf("%s ", formatted_time);
    }
    
    if (header->linkname[0] != '\0') {
      printf("%s\n", header->linkname);
    } else {
      if (header->prefix[0] != '\0') {
        snprintf(fullpath, MAX_PATH, "%s/%s", header->prefix, header->name);
      } else {
        strncpy(fullpath, header->name, MAX_NAME);
      }
      printf("%s\n", fullpath);
    }
    offset = size ? ((size / 512) + 1) : 0; /* calculate block to read */
    lseek(tarfile, offset * 512, SEEK_CUR); /* seek to next header block */
  }

  close(tarfile);
  return;
}

void print_perms(char *mode_octal) {
  char perms[11];
  unsigned long mode; 

  mode = strtoul(mode_octal, NULL, 8);
  if (S_ISDIR(mode)) {
    perms[0] = 'd';
  } else if (S_ISLNK(mode)) {
    perms[0] = 'l';
  } else {
    perms[0] = '-';
  }
  perms[1] = (mode & S_IRUSR) ? 'r' : '-';
  perms[2] = (mode & S_IWUSR) ? 'w' : '-';
  perms[3] = (mode & S_IXUSR) ? 'x' : '-';
  perms[4] = (mode & S_IRGRP) ? 'r' : '-';
  perms[5] = (mode & S_IWGRP) ? 'w' : '-';
  perms[6] = (mode & S_IXGRP) ? 'x' : '-';
  perms[7] = (mode & S_IROTH) ? 'r' : '-';
  perms[8] = (mode & S_IWOTH) ? 'w' : '-';
  perms[9] = (mode & S_IXOTH) ? 'x' : '-';
  perms[10] = '\0';
  printf("%s", perms);
}

void print_time(char *time) {

}