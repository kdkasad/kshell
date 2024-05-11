#ifndef BUILTINS_BUILTINS_H
#define BUILTINS_BUILTINS_H

typedef int (builtin_func)(char **);

struct builtin {
	char *name;
	builtin_func *func;
};

int kshell_builtin_cd(char **);
int kshell_builtin_exit(char **);

extern const struct builtin builtins[];
extern const int num_builtins;

#endif /* BUILTINS_BUILTINS_H */
