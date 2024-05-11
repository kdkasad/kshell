#include <ctype.h>
#include <pwd.h>
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
char *subst_homedir(const char *str) {

/* True iff a character is a "valid" username character/
 * This is based off of what I think should work, not on any standards. */
#define IS_NAME_CHAR(c) (isalnum(c) || ((c) == '_') || ((c) == '-'))

	if (str[0] != '~')
error:
		return strdup(str);

	if ((str[1] == '\0') || (str[1] == '/')) {
		char *homedir = get_home_dir();
		if (!homedir)
			goto error;

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
		/* Extract the name following the '~' */
		size_t namelen = 0;
		while (IS_NAME_CHAR(str[namelen + 1]))
			namelen++;
		char *name = malloc(namelen + 1);
		if (!name) {
			perror(PROGNAME": malloc");
			exit(EXIT_FAILURE);
		}
		strncpy(name, str + 1, namelen);
		name[namelen] = '\0';

		/* Get the home directory of that user */
		const struct passwd *pwent = getpwnam(name);
		if (!pwent)
			goto error;
		const char *homedir = pwent->pw_dir;

		/* Construct result string. No need to allocate extra space for the NUL
		 * terminator because the '~' will be removed. */
		char *tmp = malloc(strlen(str) - namelen + strlen(homedir));
		strcpy(tmp, homedir);
		strcat(tmp, str + 1 + namelen);
		return tmp;
	}

#undef IS_NAME_CHAR
}
