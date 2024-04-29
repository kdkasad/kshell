#include "builtins.h"
#include "../macros.h"

/*
 * Exit the shell
 */
int kshell_builtin_exit(char **args)
{
	UNUSED(args);
	return -1;
}
