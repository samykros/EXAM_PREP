#include <unistd.h>  // fork, execvp, pipe, dup2, close
#include <sys/wait.h> // wait
#include <stdlib.h>  // exit
#include <stdio.h>   // perror

int picoshell(char **cmds[])
{
	int i = 0;
	int num_cmds = 0;

	// Contar cuántos comandos hay en cmds[]
	while (cmds[num_cmds] != NULL)
		num_cmds++;

	if (num_cmds == 0)
		return 1;  // No hay comandos, error

	// Crear pipes
	int pipes[num_cmds - 1][2];
	for (i = 0; i < num_cmds - 1; i++)
	{
		if (pipe(pipes[i]) == -1)
		{
			perror("pipe");
			return 1;
		}
	}

	// Ejecutar cada comando en un proceso hijo
	for (i = 0; i < num_cmds; i++)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return 1;
		}

		if (pid == 0)
		{ // Código del hijo
			// Si no es el primer comando, redirigir stdin desde el pipe anterior
			if (i > 0)
				dup2(pipes[i - 1][0], STDIN_FILENO);

			// Si no es el último comando, redirigir stdout al pipe actual
			if (i < num_cmds - 1)
				dup2(pipes[i][1], STDOUT_FILENO);

			// Cerrar todos los pipes en el hijo
			for (int j = 0; j < num_cmds - 1; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			// Ejecutar el comando
			execvp(cmds[i][0], cmds[i]);
			perror("execvp"); // Si llega aquí, execvp falló
			exit(1);
		}
	}

	// Código del padre: cerrar todos los pipes
	for (i = 0; i < num_cmds - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	// Esperar a todos los hijos
	for (i = 0; i < num_cmds; i++)
		wait(NULL);

	return 0;
}

int main()
{
	char *cmd1[] = {"echo", "hello", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"sed", "s/a/b/g", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};

	return picoshell(cmds);
}