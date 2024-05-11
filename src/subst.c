#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subst.h"
#include "envvars.h"
#include "paths.h"

static char *subst_homedir(const char *);

char *do_substitutions(const char *str)
{
	char *step1, *step2;
	step1 = subst_homedir(str);
	step2 = subst_envvars(step1);
	free(step1);
	return step2;
}

/*
 * Replace a leading '~' with the user's home directory.
 *
 * Returned string must be freed by the caller.
 */
static char *subst_homedir(const char *str) {
	if (str[0] == '~') {
		char *homedir = get_home_dir();
		if (!homedir)
			return strdup(str);

		/* no need to allocate extra space for a nul byte
			* because the '~' will be removed */
		char *tmp = malloc(strlen(homedir) + strlen(str));
		if (!tmp) {
			perror(PROGNAME": malloc");
			exit(EXIT_FAILURE);
		}
		strcpy(tmp, homedir);
		strcat(tmp, str + 1);
		free(homedir);
		return tmp;
	} else {
		return strdup(str);
	}
}
