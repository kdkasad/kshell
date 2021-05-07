#include <stdio.h>
#include <stdlib.h>
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
		dest = get_home_dir();
		if (!dest) {
			fputs(PROGNAME": unable to get home directory\n", stderr);
			return 0;
		}
	} else {
		/* try to go to given directory */
		dest = args[1];
	}

	if (chdir(dest) < 0) {
		perror(PROGNAME": cd");
		goto done;
	}

done:
	if (!args[1])
		free(dest);

	return 0;
}
