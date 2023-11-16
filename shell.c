#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024

int is_terminal(int fd);
void execute_command(char *command, char *argv[]);

/**
 * is_terminal - checks if a fd corresponds to terminal.
 * @fd: file descriptor.
 *
 * Return: 1 if file, otherwise 0.
 */
int is_terminal(int fd)
{
	struct stat buf;

	if (fstat(fd, &buf) == -1)
	{
		perror("fstat");
		exit(EXIT_FAILURE);
	}
	return (S_ISCHR(buf.st_mode));
}

/**
 * execute_command - executes commands.
 * @command: command entered in the shell.
 * @argv: executable file name of shell.
 *
 * Return: Nothing.
 */
void execute_command(char *command, char *argv[])
{
	pid_t pid;
	int status;
	char *path;
	char *path_token;

	pid = fork();

	if (pid == 0)
	{
		char *token = strtok(command, " ");
		char *args[MAX_INPUT_SIZE];

		int i = 0;

		while (token != NULL)
		{
			args[i++] = token;
			token = strtok(NULL, " ");
		}
		args[i] = NULL;

		path = getenv("PATH");
		path_token = strtok(path, ":");

		if (i > 1)
		{
			fprintf(stderr, "%s: No such file or directory\n", argv[0]);
			_exit(EXIT_FAILURE);
		}
		while (path_token != NULL)
		{
			if (access(args[0], X_OK) == 0)
			{
				execve(args[0], args, NULL);
				fprintf(stderr, "%s: No such file or directory\n", argv[0]);
				_exit(EXIT_FAILURE);
			}
			path_token = strtok(NULL, ":");
		}
		fprintf(stderr, "%s: No such file or directory\n", argv[0]);
		_exit(EXIT_FAILURE);
	}
	else if (pid < 0)
		perror("fork");
	else
	{
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

/**
 * main - main function.
 * @argc: argument counter.
 * @argv: argument vector.
 *
 * Return: O on success.
 */
int main(int argc, char *argv[])
{
	char *input;
	size_t input_size, len;
	pid_t wpid;
	int status;

	(void)argc;
	if (is_terminal(STDIN_FILENO))
	{
		while (1)
		{
			printf("($) ");
			if (getline(&input, &input_size, stdin) == -1)
			{
				perror("getline");
				exit(EXIT_FAILURE);
			}
			len = strlen(input);
			if (len > 0 && input[len - 1] == '\n')
				input[len - 1] = '\0';

			if (strcmp(input, "exit") == 0)
			{
				fprintf(stderr, "%s: No such file or directory\n", argv[0]);
				continue;
			}
			execute_command(input, argv);
			fflush(stdin);
			clearerr(stdin);

			do
				wpid = waitpid(-1, &status, WNOHANG);
			while (wpid > 0);
		}
	}
	else
	{
		while (getline(&input, &input_size, stdin) != -1)
		{
			len = strlen(input);
			if (len > 0 && input[len - 1] == '\n')
				input[len - 1] = '\0';

			execute_command(input, argv);
			fflush(stdin);
			clearerr(stdin);

			do
				wpid = waitpid(-1, &status, WNOHANG);
			while (wpid > 0);
		}
	}
	free(input);
	return (0);
}
