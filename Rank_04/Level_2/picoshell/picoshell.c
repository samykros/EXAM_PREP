#include <unistd.h>  // fork, execvp, pipe, dup2, close
#include <sys/wait.h> // wait
#include <stdlib.h>  // exit
#include <stdio.h>   // perror


// Que no te lie: char **cmds[] == ***cmds, es un array de arrays
// de strings cmds es un array donde cada cmds[i] es otro array.
// Cada cmds[i] es un char ** (array de strings)
/*
cmds = {
	{"echo", "hi", NULL},  // cmds[0]
	{"grep", "hi", NULL},  // cmds[1]
	{"wc", "-l", NULL},    // cmds[2]
	NULL
}
*/
int picoshell(char **cmds[])
{
	int num_cmds = 0;

	// Contar cuántos comandos hay en cmds[]
	while (cmds[num_cmds] != NULL)
		num_cmds++;

	if (num_cmds == 0)
		return 1;  // No hay comandos, error

	// Crear pipes
	int pipes[num_cmds - 1][2];
	for (int i = 0; i < num_cmds - 1; i++)
	{
		if (pipe(pipes[i]) == -1) // si pipe devuelve -1 es error
		{
			perror("pipe");
			return 1;
		}
	}

	// Ejecutar cada comando en un proceso hijo
	for (int i = 0; i < num_cmds; i++)
	{
		pid_t pid = fork();
		if (pid == -1) // Error
		{
			perror("fork");
			return 1;
		}

		if (pid == 0)
		{ // Código del hijo
			// Si no es el primer comando, redirigir stdin desde el pipe anterior
			// El pipe que debe ser el stdin de un proceso es el pipe creado en
			// la iteración anterior, por eso pipes[i - 1]
			//  Resumen, pipes[i - 1][0] == "leer del pipe que escribió el comando anterior"
			if (i > 0)
				dup2(pipes[i - 1][0], STDIN_FILENO);

			// Si no es el último comando, redirigir stdout al pipe actual
			if (i < num_cmds - 1)
				dup2(pipes[i][1], STDOUT_FILENO);

			// Cerrar todos los pipes en el hijo, por memoria
			for (int j = 0; j < num_cmds - 1; j++)
			{
				close(pipes[j][0]); // Cerrar lectura del pipe
				close(pipes[j][1]); // Cerrar escritura del pipe
			}

			// Reemplaza el proceso actual con el comando ejecutado
			// Un proceso es un programa en ejecucion, acuerdate de lo que hace fork
			execvp(cmds[i][0], cmds[i]);
			// cmds[i][0] → Nombre del comando (ejemplo: "ls").
			// cmds[i] → Lista de argumentos ({"ls", "-l", NULL}).
			// Todas las listas tienen max 3 argumentos (creo), comando, comando, NULL
			// 1. El shell (picoshell) ejecuta `fork()` para `ls`, creando un proceso hijo.
			// 2. `ls` se ejecuta en ese proceso hijo usando `execvp("ls", {"ls", "-l", NULL})`.
			// 3. El shell ejecuta `fork()` para `grep`, creando otro proceso hijo.
			// 4. `grep` se ejecuta en ese proceso hijo con `execvp("grep", {"grep", "txt", NULL})`.
			// 5. El shell ejecuta `fork()` para `wc`, creando otro proceso hijo.
			// 6. `wc` se ejecuta en ese proceso hijo con `execvp("wc", {"wc", "-l", NULL})`.
			perror("execvp"); // Si llega aquí, execvp falló
			exit(1);
		}
	}

	// Código del padre: cerrar todos los pipes
	for (int i = 0; i < num_cmds - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	// Esperar a todos los hijos
	for (int i = 0; i < num_cmds; i++)
		wait(NULL);

	return 0;
}

void run_test(char *test_name, char **cmds[])
{
	printf("🔹 Test: %s\n", test_name);
		
	int result = picoshell(cmds);
		
	if (result == 0) {
		printf("✅ PASÓ: %s\n\n", test_name);
	} else {
		printf("❌ FALLÓ: %s\n\n", test_name);
	}
}

int main()
{
	// Cada test representa una tubería de comandos

	// echo hello
	char *cmd1[] = {"echo", "hello", NULL};
	char **test1[] = {cmd1, NULL};

	// echo test | cat | sed 's/t/T/g'
	char *cmd2_1[] = {"echo", "test", NULL};
	char *cmd2_2[] = {"cat", NULL};
	char *cmd2_3[] = {"sed", "s/t/T/g", NULL};
	char **test2[] = {cmd2_1, cmd2_2, cmd2_3, NULL};

	// echo squalala | cat | sed 's/a/b/g'
	char *cmd3_1[] = {"echo", "squalala", NULL};
	char *cmd3_2[] = {"cat", NULL};
	char *cmd3_3[] = {"sed", "s/a/b/g", NULL};
	char **test3[] = {cmd3_1, cmd3_2, cmd3_3, NULL};

	// ls | grep picoshell
	char *cmd4_1[] = {"ls", NULL};
	char *cmd4_2[] = {"grep", "picoshell", NULL};
	char **test4[] = {cmd4_1, cmd4_2, NULL};

	// echo hi | grep bye
	char *cmd5_1[] = {"echo", "hi", NULL};
	char *cmd5_2[] = {"grep", "bye", NULL};
	char **test5[] = {cmd5_1, cmd5_2, NULL};

	// cat /etc/passwd | wc -l
	char *cmd6_1[] = {"cat", "/etc/passwd", NULL};
	char *cmd6_2[] = {"wc", "-l", NULL};
	char **test6[] = {cmd6_1, cmd6_2, NULL};

	// /bin/false (este comando siempre falla)
	char *cmd7[] = {"/bin/false", NULL};
	char **test7[] = {cmd7, NULL};

	// /bin/false | echo ok
	char *cmd8_1[] = {"/bin/false", NULL};
	char *cmd8_2[] = {"echo", "ok", NULL};
	char **test8[] = {cmd8_1, cmd8_2, NULL};

	// Ejecutamos cada test
	run_test("echo hello", test1);
	run_test("echo test | cat | sed 's/t/T/g'", test2);
	run_test("echo squalala | cat | sed 's/a/b/g'", test3);
	run_test("ls | grep picoshell", test4);
	run_test("echo hi | grep bye", test5);
	run_test("cat /etc/passwd | wc -l", test6);
	run_test("/bin/false", test7);
	run_test("/bin/false | echo ok", test8);

	return 0;
}