#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <aio.h>
#include <stdlib.h>

int ft_popen(const char *cmd, char *const argv[], char type)
{
	pid_t pid = fork();
	if (pid == -1)
		return -1;

	int pipe_fds[2]; // Creamos el pipe, el, solo 1, no como picoshell
	if (pipe(pipe_fds) == -1)
		return -1;

	if (pid == 0)
	{
		if (type == 'r') // El padre quiere leer, así que el hijo escribe
		{
			close(pipe_fds[0]); // Cerrar lado de lectura
			dup2(pipe_fds[1], STDOUT_FILENO); // Redirigir stdout al pipe
		}
		else if (type == 'w') // El padre quiere escribir, así que el hijo lee
		{
			close(pipe_fds[1]); // Cerrar lado de escritura
			dup2(pipe_fds[0], STDIN_FILENO); // Redirigir stdin al pipe
		}
		
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		
		execvp(cmd, argv);
		exit(1); // Terminamos el hijo si execvp falla
	}

	// Código del padre
	if (type == 'r')
	{
		close(pipe_fds[1]); // Cierra escritura, devuelve lectura
		return pipe_fds[0];
	}
	else if (type == 'w')
	{
		close(pipe_fds[0]); // Cierra lectura, devuelve escritura
		return pipe_fds[1];
	}

	return -1; // En caso de error
}

int	main()
{
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	// int fd = ft_popen("cat", (char *const []){"cat", NULL}, 'w');

	char	*line;
	while ((line = get_next_line(fd)))
		ft_putstr(line);
}