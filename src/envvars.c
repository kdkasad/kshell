#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "envvars.h"
#include "string.h"

/*
 * Perform variable substitution on a string.
 *
 * This function replaces all instances of $VAR or ${VAR} with the value of the
 * environment variable VAR. If VAR is not set, it is replaced with an empty
 * string.
 *
 * A backslash before a $ character will escape it, preventing substitution.
 *
 * The returned string must be freed by the caller.
 */
char *subst_envvars(const char *str)
{
#define SE_BUFSIZ 512
#define SE_DELIM " $\t\r\n\a"

	const size_t input_strlen = strlen(str);

	size_t bufsiz = SE_BUFSIZ;
	size_t pos = 0;
	char *res;

	res = malloc(bufsiz);
	if (!res) {
		perror(PROGNAME": malloc");
		exit(EXIT_FAILURE);
	}

	/* start and end indices of the variable name/key */
	size_t keystart, keyend;
	/* index of the end of the text to be replaced */
	size_t repend;

	for (size_t i = 0; i < input_strlen; i++) {
		if (str[i] == '\\') {
			/* skip characters immediately preceded by backslashes */
			i++;
			continue;
		}

		if (str[i] == '$') {
			if (str[i + 1] == '{') {
				/* if using ${VAR} syntax, scan to matching '}' */
				keystart = i + 2;
				const char *rbrace = strchr(str + keystart, '}');
				if (!rbrace) {
					fputs(PROGNAME": invalid syntax: expected '}'\n", stderr);
					return NULL;
				}
				keyend = rbrace - (str + keystart);
				repend = keyend + 1;
			} else {
				/* using $VAR syntax, scan until whitespace or end of string */
				keystart = i + 1;
				char *endchar = strpbrk(str + keystart, SE_DELIM);
				if (endchar)
					keyend = repend = endchar - str;
				else
					keyend = repend = input_strlen;
			}

			/* done scanning, time to replace */
			char *key, *val;
			key = strndup(str + keystart, keyend - keystart);
			val = getenv(key);
			free(key);
			if (val && *val) {
				size_t val_len = strlen(val);
				if ((pos + val_len) >= bufsiz) {
					bufsiz += val_len;
					res = realloc(res, bufsiz);
					if (!res) {
						perror(PROGNAME": malloc");
						exit(EXIT_FAILURE);
					}
				}
				strcat(res, val);
				pos += val_len;
			}
			/* -1 because i++ at end of loop */
			i = repend - 1;
		} else {
			/* append character to result string */
			res[pos++] = str[i];
		}

		/* if buffer is full, reallocate a larger one */
		if (pos >= bufsiz) {
			bufsiz += SE_BUFSIZ;
			res = realloc(res, bufsiz);
			if (!res) {
				perror(PROGNAME": malloc");
				exit(EXIT_FAILURE);
			}
		}
	}

	res[pos] = '\0';

	return res;

#undef SE_BUFSIZ
#undef SE_DELIM
}
