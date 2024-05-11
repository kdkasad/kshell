#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"
#include "paths.h"
#include "prompt.h"
#include "string.h"

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
		prompt = strdup(ps1);
	} else {
		/* Path + " $ " + NUL terminator */
		prompt = malloc(PATH_MAX + 4);
		if (!prompt) {
			perror(PROGNAME": malloc");
			exit(EXIT_FAILURE);
		}

		/* store current directory into prompt */
		if (!getcwd(prompt, PATH_MAX + 1)) {
			perror(PROGNAME": getcwd");
			prompt[0] = '\0';
		}

		/* replace home dir with '~' */
		const char *home = get_home_dir(NULL);
		const size_t home_len = strlen(home);
		if (home && !strncmp(prompt, home, home_len)) {
			memmove(prompt + 1, prompt + home_len, strlen(prompt) - home_len + 1);
			prompt[0] = '~';
		}
		free((void *) home);

		/* append '#' for root or '$' for any other user */
		if (geteuid() == 0)
			strcat(prompt, " # ");
		else
			strcat(prompt, " $ ");
	}

	return prompt;
}
