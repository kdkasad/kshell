#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct builtin {
	char *name;
	int (*func)(char **);
};

static int kshell_builtin_cd(char **args);
static int kshell_builtin_exit(char **args);
static int kshell_execute(char **args);
static int kshell_launch(char **args);
static void kshell_loop();
static char *kshell_read_line();
static char **kshell_split_line(char *line);

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

/*
 * Exit the shell
 */
int kshell_builtin_exit(char **args)
{
	return -1;
}

/*
 * Execute a program with the given arguments
 */ 
int kshell_execute(char **args)
{
	pid_t pid;

	pid = fork();
	if (pid == 0) { /* child process */
		if (execvp(args[0], args) == -1) {
			perror("kshell: execvp");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) { /* error forking */
		perror("kshell: fork");
		exit(EXIT_FAILURE);
	} else { /* parent process */
		int status;
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 0;
}

/*
 * Perform the action of a command
 */
int kshell_launch(char **args)
{
	if (!args[0])
		return 0;

	/* if the given command is a builtin, run that function */
	for (int i = 0; i < sizeof(builtins)/sizeof(builtins[0]); i++) {
		if (!strcmp(args[0], builtins[i].name)) {
			return builtins[i].func(args);
		}
	}

	/* otherwise, execute the program */
	return kshell_execute(args);
}

/*
 * Main program loop
 */
void kshell_loop()
{
	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = kshell_read_line();
		args = kshell_split_line(line);
		status = kshell_launch(args);

		free(line);
		free(args);
	} while (!status);
}

/*
 * Read a line of input from stdin
 *
 * Returned string must be freed.
 */
char *kshell_read_line()
{
#define RL_BUFFER_SIZE 1024

	size_t bufsize = RL_BUFFER_SIZE; /* size of buffer */
	size_t pos = 0; /* current position in buffer */
	char *buffer = malloc(bufsize); /* input buffer */
	int c;

	if (!buffer) {
		perror("kshell: malloc");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		c = getchar();

		/*
		 * If end of line is reached, terminate the buffer with a NUL
		 * and return. If EOF is reached, exit only if the current line
		 * is empty. If EOF is reached and the line is not empty,
		 * process like EOL.
		 */
		if (c == '\n') {
			buffer[pos] = '\0';
			return buffer;
		} else if (c == EOF) {
			if (pos == 0)
				exit(EXIT_SUCCESS);
			buffer[pos] = '\0';
			return buffer;
		}

		/* Otherwise, add the character to the buffer and continue. */
		buffer[pos++] = (char) c;

		/* If the buffer is full, reallocate with more space */
		if (pos >= bufsize) {
			bufsize += RL_BUFFER_SIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				perror("kshell: realloc");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/*
 * Split a line of input into a list of arguments
 *
 * Result must be freed.
 */
char **kshell_split_line(char *line)
{
#define SL_BUFFER_SIZE 64
#define SL_TOK_DELIM " \t\r\n\a"
	
	size_t bufsize = SL_BUFFER_SIZE, pos = 0;
	char **tokens = calloc(bufsize, sizeof(char*));
	char *token;

	if (!tokens) {
		perror("kshell: malloc");
		exit(EXIT_FAILURE);
	}

	for (token = strtok(line, SL_TOK_DELIM); token;
			token = strtok(NULL, SL_TOK_DELIM)) {
		tokens[pos++] = token;

		if (pos >= bufsize) {
			bufsize += SL_BUFFER_SIZE;
			tokens = realloc(tokens, bufsize);
			if (!tokens) {
				perror("kshell: realloc");
				exit(EXIT_FAILURE);
			}
		}
	}

	tokens[pos] = NULL;
	return tokens;
}

int main(int argc, char *argv[])
{
	kshell_loop();

	return EXIT_SUCCESS;
}
