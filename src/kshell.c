#define _XOPEN_SOURCE 700

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins/builtins.h"
#include "macros.h"

static int kshell_execute(char **args);
static int kshell_launch(char **args);
static void kshell_loop(FILE *f);
static char *kshell_read_line(FILE *f);
static char **kshell_split_line(char *line);

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
	for (int i = 0; i < num_builtins; i++) {
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
void kshell_loop(FILE *f)
{
	char *line;
	char **args;
	int status;

	do {
		/* only print prompt if reading from stdin */
		TODO("only print prompt if reading from terminal");
		if (f == stdin)
			printf("> ");
		line = kshell_read_line(f);
		if (!line)
			return;
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
char *kshell_read_line(FILE *f)
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
		c = getc(f);

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
				return NULL;
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

	if (argc > 1) {
		FILE *f;
		for (int i = 1; i < argc; i++) {
			f = fopen(argv[i], "r");
			if (!f) {
				fprintf(stderr, "unable to open file: %s\n", strerror(errno));
				return 1;
			}
			kshell_loop(f);
			fclose(f);
		}
	} else {
		kshell_loop(stdin);
	}

	return EXIT_SUCCESS;
}
