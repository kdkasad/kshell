#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "paths.h"
#include "subst.h"
#include "text.h"

/*
 * Split a line of input into a list of arguments
 *
 * Each string in the returned array, as well as the array itself, must be
 * freed by the caller. Use the helper function kshell_free_split_line(...) to
 * do so.
 */
char **kshell_split_line(char *line)
{
#define SL_BUFFER_SIZE 64
#define SL_TOK_DELIM " \t\r\n\a"

	char *token;
	/* allocate buffer for array of tokens */
	size_t bufsize = SL_BUFFER_SIZE, pos = 0;
	char **tokens = calloc(bufsize, sizeof(char *));

	if (!tokens) {
		perror(PROGNAME": malloc");
		exit(EXIT_FAILURE);
	}

	/* for each token in the line */
	TODO("split line without modifying it");
	for (token = strtok(line, SL_TOK_DELIM); token;
			token = strtok(NULL, SL_TOK_DELIM)) {
		if (token[0] == '~') {
			/* replace '~' with home directory */
			char *homedir = get_home_dir();
			if (!homedir)
				goto no_home_dir_subst;
			/* no need to allocate extra space for a nul byte
			 * because the '~' will be removed */
			char *tmp = malloc(strlen(homedir) + strlen(token));
			strcpy(tokens[pos], homedir);
			strcat(tokens[pos], token + 1);
			free(homedir);
			tokens[pos++] = do_substitutions(tmp);
			free(tmp);
		} else {
no_home_dir_subst:
			/* allocate a new string for the token */
			tokens[pos++] = do_substitutions(token);
		}

		/* if the token array is full, reallocate a larger one */
		if (pos >= bufsize) {
			bufsize += SL_BUFFER_SIZE;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens) {
				perror(PROGNAME": realloc");
				exit(EXIT_FAILURE);
			}
		}
	}

	/* terminate array */
	tokens[pos] = NULL;

	return tokens;

#undef SL_BUFFER_SIZE
#undef SL_TOK_DELIM
}
