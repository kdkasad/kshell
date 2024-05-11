#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subst.h"
#include "envvars.h"
#include "paths.h"
#include "string.h"

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
 * Replaces the '~' character at the beginning of a string with the home
 * directory of the user specified by the following characters. If the '~' is
 * followed by a '/' or the end of the string, the current user's home directory
 * is used.
 *
 * If replacement cannot be performed, a copy of the original string is returned.
 *
 * The returned string must be freed by the caller.
 */
char *subst_homedir(const char *str) {

	/* True iff a character is a "valid" username character/
	* This is based off of what I think should work, not on any standards. */
#define IS_NAME_CHAR(c) (isalnum(c) || ((c) == '_') || ((c) == '-'))

	if (str[0] != '~')
error:
		return strdup(str);

	size_t namelen = 0;
	char *username = NULL;
	if ((str[1] == '\0') || (str[1] == '/')) {
		username = NULL;
	} else if (IS_NAME_CHAR(str[1])) {
		/* Extract username following '~' */
		while (IS_NAME_CHAR(str[namelen + 1]))
			namelen++;
		username = malloc(namelen + 1);
		strncpy(username, str + 1, namelen);
		username[namelen] = '\0';
	} else {
		goto error;
	}

	const char *homedir = get_home_dir(username);
	if (username)
		free(username);

	if (!homedir)
		goto error;

	/* We don't need an extra byte for the NUL terminator because the '~' will
	 * be replaced. */
	char *res = malloc(strlen(str) - namelen + strlen(homedir));
	strcpy(res, homedir);
	strcat(res, str + 1 + namelen);

	free((void *) homedir);

	return res;

#undef IS_NAME_CHAR
}
