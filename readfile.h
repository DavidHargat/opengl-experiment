#include <stdlib.h>
#include <stdio.h>

#ifndef READFILE_H
#define READFILE_H

/*
 * If readfile fails we want to
 * close the file descriptor AND return NULL.
 * So we have a function which does this
 * (instead of using a goto, or rewriting it a bunch of times)
 */
char *readfile_error(FILE *f){
	fclose(f);
	return NULL;
}

/* RETURN VALUE
 *   success:
 *     returns STRING with the file contents, with an additional null-byte at the end.
 *   error:
 *     returns NULL
 */
char *readfile(char *filename){
	char *buffer;
	long length;
	int result;
	FILE *f;

	f = fopen(filename, "r");         if(f==NULL)      return readfile_error(f);
	result = fseek(f, 0L, SEEK_END);  if(result==-1)   return readfile_error(f);
	length = ftell(f);                if(length==-1)   return readfile_error(f);
	result = fseek(f, 0L, SEEK_SET);  if(result==-1)   return readfile_error(f);
	buffer = malloc(length+1);        if(buffer==NULL) return readfile_error(f);
	fread(buffer, sizeof(char), length, f);

	// Null byte at end of string.
	buffer[length] = '\0';
	fclose(f);

	return buffer;
}

#endif
