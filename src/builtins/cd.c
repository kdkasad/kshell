#include <stdio.h>
#include <unistd.h>

#include "builtins.h"

/*
 * Change directory builtin command
 */
int kshell_builtin_cd(char **args)
{
	if (!args[1]) {
		fprintf(stderr, "kshell: cd: expected argument\n");
	} else if (chdir(args[1]) < 0) {
		perror("kshell: cd");
	}

	return 0;
}
