#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "paths.h"

/*
 * Get the given user's home directory.
 *
 * If the username argument is a NULL pointer, the home directory of the current
 * user is found.
 *
 * For the current user, the $HOME environment variable is checked first, and
 * the passwd database is used as a fallback.
 *
 * For other users, the passwd database is used.
 *
 * Returns NULL if the home directory cannot be resolved.
 * Returned string must be freed by the caller.
 */
char *get_home_dir(const char *username)
{
	const struct passwd *pw;

	if (!username) {
		const char *home = getenv("HOME");
		if (home && *home) {
			return strdup(home);
		} else {
			pw = getpwuid(geteuid());
		}
	} else {
		pw = getpwnam(username);
	}

	if (!pw || !pw->pw_dir || !*pw->pw_dir)
		return NULL;

	return strdup(pw->pw_dir);
}
