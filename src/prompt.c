#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"
#include "paths.h"
#include "prompt.h"

#define PROMPT_BUF_SIZ 256

/*
 * Get the prompt text
 *
 * If $PS1 is set, use that as the prompt. Otherwise, the current directory,
 * followed by a dollar sign ('$') is used.
 *
 * The returned string is heap-allocated, and must be freed by the caller.
 */
char *get_prompt_text(void)
{
	char *prompt;

	/* if $PS1 environment variable is set, use that as the prompt */
	char *ps1 = getenv("PS1");
	if (ps1 && *ps1) {
		prompt = malloc(strlen(ps1) + 1);
		if (!prompt) {
			perror(PROGNAME": malloc");
			exit(EXIT_FAILURE);
		}
		strcpy(prompt, ps1);
	} else {
		prompt = malloc(PATH_MAX);
		if (!prompt) {
			perror(PROGNAME": malloc");
			exit(EXIT_FAILURE);
		}

		/* store current directory into prompt */
		if (getcwd(prompt, PATH_MAX - 4) == NULL) {
			perror(PROGNAME": getcwd");
			prompt[0] = '\0';
		}

		/* replace home dir with '~' */
		char *home = get_home_dir();
		if (home && !strncmp(prompt, home, strlen(home))) {
			memmove(prompt + 1, prompt + strlen(home), strlen(prompt));
			prompt[0] = '~';
		}

		/* append '#' for root or '$' for any other user */
		if (geteuid() == 0)
			strcat(prompt, " # ");
		else
			strcat(prompt, " $ ");
	}

	return prompt;
}
