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
#include "paths.h"
#include "prompt.h"
#include "subst.h"

static int kshell_execute(char **args);
static void kshell_free_split_line(char **args);
static int kshell_launch(char **args);
static void kshell_loop(FILE *f);
static char *kshell_read_line(FILE *f);
static int kshell_process_line(char *line);
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
			perror(PROGNAME": execvp");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) { /* error forking */
		perror(PROGNAME": fork");
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
 * Free the list of arguments returned by kshell_split_line(...)
 */
void kshell_free_split_line(char **args)
{
	for (int i = 0; args[i]; i++) {
		free(args[i]);
	}
	free(args);
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
	char *prompt;
	int status;

	do {
		/* print prompt if reading from a terminal */
		prompt = get_prompt_text();
		if (isatty(fileno(f)))
			fputs(prompt, stderr);
		free(prompt);

		/* read line and process it */
		line = kshell_read_line(f);
		if (!line)
			return;
		status = kshell_process_line(line);
		free(line);
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
		perror(PROGNAME": malloc");
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
				perror(PROGNAME": realloc");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/*
 * Process and run a line
 */
static int kshell_process_line(char *line)
{
	char **args;
	int status;

	args = kshell_split_line(line);

	/* interpret and run the line */
	status = kshell_launch(args);

	kshell_free_split_line(args);

	return status;
}

/*
 * Split a line of input into a list of arguments
 *
 * Each string in the returned array, as well as the array itself, must be
 * freed by the caller. Use the helper function kshell_free_split_line(...) to
 * do so.
 */
char **kshell_split_line(char *line)
{
#define SL_BUFFER_SIZE 64
#define SL_TOK_DELIM " \t\r\n\a"
	
	char *token;
	/* allocate buffer for array of tokens */
	size_t bufsize = SL_BUFFER_SIZE, pos = 0;
	char **tokens = calloc(bufsize, sizeof(char *));

	if (!tokens) {
		perror(PROGNAME": malloc");
		exit(EXIT_FAILURE);
	}

	/* for each token in the line */
	for (token = strtok(line, SL_TOK_DELIM); token;
			token = strtok(NULL, SL_TOK_DELIM)) {
		if (token[0] == '~') {
			/* replace '~' with home directory */
			char *homedir = get_home_dir();
			if (!homedir)
				goto no_home_dir_subst;
			/* no need to allocate extra space for a nul byte
			 * because the '~' will be removed */
			char *tmp = malloc(strlen(homedir) + strlen(token));
			strcpy(tokens[pos], homedir);
			strcat(tokens[pos], token + 1);
			free(homedir);
			tokens[pos++] = do_substitutions(tmp);
			free(tmp);
		} else {
no_home_dir_subst:
			/* allocate a new string for the token */
			tokens[pos++] = do_substitutions(token);
		}

		/* if the token array is full, reallocate a larger one */
		if (pos >= bufsize) {
			bufsize += SL_BUFFER_SIZE;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens) {
				perror(PROGNAME": realloc");
				exit(EXIT_FAILURE);
			}
		}
	}

	/* terminate array */
	tokens[pos] = NULL;

	return tokens;
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		FILE *f;
		for (int i = 1; i < argc; i++) {
			if (!strcmp(argv[i], "-c")) {
				kshell_process_line(argv[++i]);
			} else {
				f = fopen(argv[i], "r");
				if (!f) {
					fprintf(stderr, "unable to open file: %s\n", strerror(errno));
					return 1;
				}
				kshell_loop(f);
				fclose(f);
			}
		}
	} else {
		kshell_loop(stdin);
	}

	return EXIT_SUCCESS;
}
