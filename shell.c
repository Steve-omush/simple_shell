#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_COMMAND_LENGTH 1024

void display_prompt(void);
void execute_command(char *command);

/**
 * display_prompt - display the shell.
 *
 * Return: Nothing.
 */
void display_prompt(void)
{
	printf("#cisfun$ ");
	fflush(stdout);
}

/**
 * execute_command - executes commands in shell.
 * @command: executable command.
 *
 * Return: Nothing.
 */
void execute_command(char *command)
{
	pid_t pid;
	int status;

	char *args[2];
	args[0] = command;
	args[1] = NULL;

	pid = fork();
	if (pid == 0)
	{
		execve(args[0], args, NULL);
		dprintf(STDERR_FILENO, "./shell: No such file or directory\n");
		_exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
	}
	else
	{
		dprintf(STDERR_FILENO, "./shell: No such file or directory\n");
	}
}

/**
 * main: main function.
 *
 * Return: 0 Success.
 */
int main(void)
{
	char *command = NULL;
	size_t len = 0;
	ssize_t read;

	while (1)
	{
		display_prompt();

		if ((read = getline(&command, &len, stdin)) == -1)
		{
			free(command);
			exit(EXIT_SUCCESS);
		}

		if (read > 0 && command[read - 1] == '\n')
		{
			command[read - 1] = '\0';
		}

		if (strcmp(command, "exit") == 0)
		{
			free(command);
			exit(EXIT_SUCCESS);
		}

		execute_command(command);
	}
	return (0);
}	
