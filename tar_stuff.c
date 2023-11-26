#include "tar_stuff.h"

/* takes string of options, iterates through options and turns
 * on corresponding flags. returns void */
void check_options(char *options, Options *opts) {
	int i;
    
    opts->c = 0;
    opts->t = 0;
    opts->x = 0;
    opts->v = 0;
    opts->f = 0;
    opts->S = 0;

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
				fprintf(stderr,
                        "not a valid option: %c\n", options[i]);
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
		fprintf(stderr,
                "options must contain one and only one of: 'c', 't', 'x'\n");
		exit(EXIT_FAILURE);
	}
    return;
}

/* function to store octal string of val into dest */
void int_to_octal(char *dest,int size, unsigned long val){
	snprintf(dest, size, "%0*lo", size-1, val);
}



/* For interoperability with GNU tar. GNU seems to
* set the high–order bit of the first byte, then
* treat the rest of the field as a binary integer
* in network byte order.
* I don’t know for sure if it’s a 32 or 64–bit int, but for
* this version, we’ll only support 32. (well, 31)
* returns the integer on success, –1 on failure.
* In spite of the name of htonl(), it converts int32 t
*/
uint32_t extract_special_int(char *where, unsigned int len) {
	int32_t val= -1;
	if ( (len >= sizeof(val)) && (where[0] & 0x80)) {
		/* the top bit is set and we have space
		* extract the last four bytes */
		val = *(int32_t *)(where+len-sizeof(val));
		val = ntohl(val); /* convert to host byte order */
	}
	return val;
}

/* Insert the given integer into the given field
* using this technique. Returns 0 on success, nonzero
* otherwise */
int insert_special_int(char *where, size_t size, int32_t val) {
	int err=0;
	if ( val < 0 || ( size < sizeof(val)) ) {
		/* if it’s negative, bit 31 is set and we can’t use the flag
		* if len is too small, we can’t write it. Either way, we’re
		* done.
		*/
		err++;
	} else {
	/* game on....*/
	memset(where, 0, size); /* Clear out the buffer */
	*(int32_t *)(where+size-sizeof(val)) = htonl(val); /* place the int */
	*where |= 0x80; /* set that high–order bit */
	}
	return err;
}
/* fills and returns a header struct from a passed in header buffer
 * from a tarfile*/
Header *readHeader(uint8_t *buf){
    Header *header; /* header */

    /* malloc space for Header struct */  
    if ((header = malloc(sizeof(Header))) == NULL) {
        perror("malloc on Header struct");
        exit(EXIT_FAILURE);
    }

    memcpy(header->name, &buf[NAME_OFFSET], MAX_NAME); /* name */
    memcpy(header->mode, &buf[MODE_OFFSET], MAX_MODE); /* mode */
    memcpy(header->uid, &buf[UID_OFFSET], MAX_ID); /* uid */
    memcpy(header->gid, &buf[GID_OFFSET], MAX_ID); /* gid */
    memcpy(header->size, &buf[SIZE_OFFSET], MAX_SIZE); /* size */
    memcpy(header->mtime, &buf[MTIME_OFFSET], MAX_MTIME); /* mtime */
    memcpy(header->chksum, &buf[CHKSUM_OFFSET], MAX_CHKSUM); /* chksum */
    memcpy(header->typeflag,
            &buf[TYPEFLAG_OFFSET], MAX_TYPEFLAG); /* typeflag */
    memcpy(header->linkname,
            &buf[LINKNAME_OFFSET], MAX_LINKNAME); /* linkname */
    memcpy(header->magic, &buf[MAGIC_OFFSET], MAX_MAGIC); /* magic */
    memcpy(header->version, &buf[VERSION_OFFSET], MAX_VERSION); /* version */
    memcpy(header->uname, &buf[UNAME_OFFSET], MAX_UNAME); /* uname */
    memcpy(header->gname, &buf[GNAME_OFFSET], MAX_GNAME); /* gname */
    memcpy(header->devmajor, &buf[DEVMAJ_OFFSET], MAX_DEVMAJ); /* devmajor */
    memcpy(header->devminor, &buf[DEVMIN_OFFSET], MAX_DEVMIN); /* devminor */
    memcpy(header->prefix, &buf[PREFIX_OFFSET], MAX_PREFIX); /* prefix */

    return header;
}
