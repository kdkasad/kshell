#include <stdio.h>
#include <unistd.h>

#include "builtins.h"

/*
 * Change directory builtin command
 */
int kshell_builtin_cd(char **args)
{
	if (!args[1]) {
		fprintf(stderr, PROGNAME": cd: expected argument\n");
	} else if (chdir(args[1]) < 0) {
		perror(PROGNAME": cd");
	}

	return 0;
}
