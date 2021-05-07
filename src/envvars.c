#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "envvars.h"

char *subst_envvars(const char *str)
{
#define SE_BUFSIZ 512
#define SE_DELIM " $\t\r\n\a"

	const size_t instrlen = strlen(str);

	size_t bufsiz = SE_BUFSIZ;
	size_t pos = 0;
	char *res = calloc(bufsiz, sizeof(char));

	/* start and end indices of the variable name/key */
	size_t keystart, keyend;
	/* index of the end of the text to be replaced */
	size_t repend;

	for (size_t i = 0; i < instrlen; i++) {
		if (str[i] == '\\') {
			/* skip characters immediately preceded by backslashes */
			i++;
			continue;
		}

		if (str[i] == '$') {
			if (str[i + 1] == '{') {
				/* if using ${VAR} syntax, scan to matching '}' */
				keystart = i + 2;
				for (size_t j = keystart; j < instrlen; j++) {
					if (str[j] == '}') {
						keyend = j;
						repend = j + 1;
						goto done_scanning;
					}
				}
				fputs(PROGNAME": invalid syntax: expected '}'\n", stderr);
				return NULL;
			} else {
				/* using $VAR syntax, scan until whitespace or end of string */
				keystart = i + 1;
				char *endchar = strpbrk(str + keystart, SE_DELIM);
				if (endchar)
					keyend = repend = endchar - str;
				else
					keyend = repend = instrlen;
			}

done_scanning:
			;
			/* done scanning, time to replace */
			char *key, *val;
			key = strndup(str + keystart, keyend - keystart);
			val = getenv(key);
			free(key);
			if (val) {
				if (strlen(val) > bufsiz - pos) {
					bufsiz = pos + strlen(val) + 1;
					res = realloc(res, bufsiz);
				}
				strcat(res, val);
				pos += strlen(val);
			}
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
				perror(PROGNAME": realloc");
				exit(EXIT_FAILURE);
			}
		}
	}

	res[pos] = '\0';

	return res;
}
