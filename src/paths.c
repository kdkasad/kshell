#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "paths.h"

/*
 * Get the user's home directory
 *
 * Returned string must be freed by the caller
 */
char *get_home_dir()
{
	char *homedir;

	/* attempt to get home dir from environment variables */
	homedir = getenv("HOME");
	if (homedir && *homedir)
		return strdup(homedir);

	/* get home dir from passwd db */
	struct passwd *pwent = getpwuid(geteuid());
	if (!pwent) {
		perror(PROGNAME": getpwuid");
		return NULL;
	}
	return strdup(pwent->pw_dir);

	return homedir;
}
