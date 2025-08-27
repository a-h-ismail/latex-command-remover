// Credits to https://stackoverflow.com/a/44894946

#ifndef READALL_H
#define READALL_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/* Size of each input chunk to be
   read and allocate for. */
#ifndef READALL_CHUNK
#define READALL_CHUNK 2097152
#endif

#define READALL_OK 0       /* Success */
#define READALL_INVALID -1 /* Invalid parameters */
#define READALL_ERROR -2   /* Stream error */
#define READALL_TOOMUCH -3 /* Too much input */
#define READALL_NOMEM -4   /* Out of memory */

/* This function returns one of the READALL_ constants.
   If the return value is READALL_OK, then:
     (*dataptr) points to a dynamically allocated buffer, with
     (*sizeptr) chars read from the file.
     The buffer is allocated for one extra char, which is NUL,
     and automatically appended after the data.
   Initial values of (*dataptr) and (*sizeptr) are ignored.
*/
int readall(FILE *in, char **dataptr, size_t *sizeptr);

#endif