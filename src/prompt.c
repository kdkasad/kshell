#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prompt.h"
#include "macros.h"

#define PROMPT_BUF_SIZ 256

/*
 * Get the prompt text
 *
 * If $PS1 is set, use that as the prompt. Otherwise, the current directory,
 * followed by a dollar sign ('$') is used.
 *
 * The returned string is heap-allocated, and must be freed by the caller.
 */
char *get_prompt_text()
{
	char *prompt;

	/* if $PS1 environment variable is set, use that as the prompt */
	char *ps1 = getenv("PS1");
	if (ps1 && *ps1) {
		prompt = malloc(strlen(ps1) + 1);
		strcpy(prompt, ps1);
	} else {
		prompt = malloc(PATH_MAX);

		/* store current directory into prompt */
		if (getcwd(prompt, PATH_MAX - 4) == NULL) {
			perror("kshell: getcwd");
			prompt[0] = '\0';
		}

		/* replace home dir with '~' */
		TODO("fall back to getting home dir from passwd db");
		char *home = getenv("HOME");
		if (home && *home && !strncmp(prompt, home, strlen(home))) {
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
