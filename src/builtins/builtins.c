#include "builtins.h"

const struct builtin builtins[] = {
	{
		.name = "cd",
		.func = &kshell_builtin_cd,
	},
	{
		.name = "exit",
		.func = &kshell_builtin_exit,
	},
};

const int num_builtins = sizeof(builtins) / sizeof(*builtins);
