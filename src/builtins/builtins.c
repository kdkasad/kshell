#include "builtins.h"

struct builtin builtins[] = {
	{
		.name = "cd",
		.func = &kshell_builtin_cd,
	},
	{
		.name = "exit",
		.func = &kshell_builtin_exit,
	},
};

int num_builtins = sizeof(builtins) / sizeof(*builtins);
