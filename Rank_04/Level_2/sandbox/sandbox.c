#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

// Función principal
int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	pid_t pid = fork();
	if (pid < 0) // Error al crear fork (pid < 0)
	{
		if (verbose)
			printf("Error: fork failed\n");
		return -1; // Error al crear el proceso
	}

	if (pid == 0) // Código del hijo (pid == 0)
	{
		alarm(timeout);  // Si f() termina antes del tiempo, todo bien. Si es bucle infinito, alarm() lo matará.
		f();			// Ejecutamos la función
		exit(0);		// Salida normal si la función es "Nice"
	}

	// Código del padre: esperar al hijo y analizar qué pasó
	int status;
	pid_t result = waitpid(pid, &status, 0);

	if (result == -1) // Error al asignar result
	{
		if (verbose)
			printf("Error: waitpid failed\n");
		return -1; // Error en waitpid
	}

	// Revisar si el hijo terminó normalmente con exit(0)
	if (WIFEXITED(status)) // status es true == bien, false == mal
	{
		int exit_code = WEXITSTATUS(status);
		if (exit_code == 0) // exit_code == 0 == bien, exit_code != 0 == mal
		{
			if (verbose)
				printf("Nice function!\n");
			return 1;
		}
		else
		{
			if (verbose)
				printf("Bad function: exited with code %d\n", exit_code);
			return 0;
		}
	}

	// Revisar si el hijo terminó por una señal
	if (WIFSIGNALED(status)) // status es true == bien, false == mal
	{
		int sig = WTERMSIG(status);
		if (verbose)
			printf("Bad function: %s\n", strsignal(sig));
		return 0;
	}

	// Revisar si el hijo fue detenido (debería ser raro aquí)
	if (WIFSTOPPED(status))
	{
		if (verbose)
			printf("Bad function: stopped\n");
		return 0;
	}

	return -1; // Caso inesperado
}

// Funciones de prueba
void nice_function() { printf("Ejecutando nice function...\n"); }
void bad_exit_function() { exit(1); }
void segfault_function() { *(int *)0 = 0; } // Provoca SIGSEGV
void infinite_loop() { while (1); } // Nunca termina

int main()
{
	printf("Prueba 1: nice_function\n");
	sandbox(nice_function, 2, true);

	printf("Prueba 2: bad_exit_function\n");
	sandbox(bad_exit_function, 2, true);

	printf("Prueba 3: segfault_function\n");
	sandbox(segfault_function, 2, true);

	printf("Prueba 4: infinite_loop (debería hacer timeout)\n");
	sandbox(infinite_loop, 2, true);

	return 0;
}