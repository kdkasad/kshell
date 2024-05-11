#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "../paths.h"

/*
 * Change directory builtin command
 */
int kshell_builtin_cd(char **args)
{
	char *dest;

	if (!args[1]) {
		/* if no directory given, go to home directory */
		dest = get_home_dir(NULL);
		if (!dest) {
			fputs(PROGNAME": unable to get home directory\n", stderr);
			return 0;
		}
	} else if (!strcmp(args[1], "-")) {
		dest = getenv("OLDPWD");
	} else {
		/* try to go to given directory */
		dest = args[1];
	}

	int chdir_ret = chdir(dest);

	if (!args[1])
		free(dest);

	if (chdir_ret < 0) {
		perror(PROGNAME": cd");
		return 0;
	}

	/* update the $PWD and $OLDPWD environment variables */
	char *cwd = malloc(PATH_MAX);
	if (!cwd) {
		perror(PROGNAME": malloc");
		exit(EXIT_FAILURE);
	}
	setenv("OLDPWD", getenv("PWD"), 1);
	if (getcwd(cwd, PATH_MAX)) {
		setenv("PWD", cwd, 1);
		free(cwd);
	} else {
		setenv("PWD", dest, 1);
	}

	return 0;
}
