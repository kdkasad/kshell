#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

/*
 * Copy a string.
 *
 * Checks for malloc(3) failure.
 */
char *strdup(const char *str) {
	char *new = malloc(strlen(str) + 1);
	if (!new) {
		perror(PROGNAME": malloc");
		exit(EXIT_FAILURE);
	}
	strcpy(new, str);
	return new;
}

/*
 * Copy a portion of a string.
 *
 * Copies at most n characters, always NUL terminating the new string.
 *
 * Checks for malloc(3) failure.
 */
char *strndup(const char *str, size_t n) {
    char *new = malloc(n + 1);
    if (!new) {
        perror(PROGNAME": malloc");
        exit(EXIT_FAILURE);
    }
    strncpy(new, str, n);
    new[n] = '\0';
    return new;
}
