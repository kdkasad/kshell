#ifndef BUILTINS_BUILTINS_H
#define BUILTINS_BUILTINS_H

typedef int (builtin_func)(char **);

struct builtin {
	char *name;
	builtin_func *func;
};

int kshell_builtin_cd(char **args);
int kshell_builtin_exit();

extern struct builtin builtins[];
extern int num_builtins;

#endif /* BUILTINS_BUILTINS_H */
