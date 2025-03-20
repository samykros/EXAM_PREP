// Crear pipes
int pipes[num_cmds - 1][2];

Ejemplo con 4 pipes

Pipe		pipes[i][0] (lectura)	pipes[i][1] (escritura)
pipes[0]	Recibe de cmd1			Envía a cmd2
pipes[1]	Recibe de cmd2			Envía a cmd3
pipes[2]	Recibe de cmd3			Envía a cmd4

Que hace int pipe(int *__pipedes)
Crea un par de descriptores de archivo (fds[2]).
fds[0] → Lado de lectura (entrada de la tubería).
fds[1] → Lado de escritura (salida de la tubería).
Devuelve 0 si tiene éxito, -1 si hay error.

Al final piensa que pid_t pid = fork(); es basicamente
un numero random. Y padre es siempre 0, lo que significa
que si pid == 0, estas en un hijo, lee mas abajo y
entenderas mejor. No compliques pid_t, ea un int,
pid nombre de variable. fork() funcionque te da un
numero random. Pero, acuerdate que un fork() 2
hace que de hacer una cosa a la vez (un proceso), tenemos
procesos. como si estas cortando una patata pero
ahora las pelas y cortas a la vez. -= visual. Y que
el pid devuelve un valor diferente en cada proceso.
En el padre, fork() devuelve el PID del hijo (número
positivo mayor que 0). En el hijo, fork() devuelve 0.

dup2() redirige un descriptor de archivo, texto basicamente
(fd) a otro. dup2(fd_origen, fd_destino);
Hace que fd_destino apunte a fd_origen.
Después de dup2(), todo lo que vaya a fd_destino irá a fd_origen.

// STDIN == entrada ==  teclado
// STDOUT == salida == pantalla

Nombre			Valor	¿Qué representa?
STDIN_FILENO	0		Entrada estándar (teclado).
STDOUT_FILENO	1		Salida estándar (pantalla).
STDERR_FILENO	2		Salida de errores (pantalla).

Cuando ejecutas ls en la terminal, su salida, pantalla va a STDOUT_FILENO (1).
Cuando ejecutas cat, su entrada, teclado, proviene de STDIN_FILENO (0).

write(STDOUT_FILENO, "Hola mundo!\n", 12); == write(1, "Hola mundo!\n", 12);

Cada hijo solo usa su entrada y salida, pero todos los pipes siguen
abiertos en cada proceso. Si no cerramos los pipes, se acumulan y
pueden causar errores. El padre también debe cerrar los pipes,
pero eso se hace fuera del fork().

execvp() reemplaza el proceso actual con el nuevo comando.
Si execvp() es exitoso, NADA después se ejecuta (ni perror(), ni exit()).
Si execvp() falla, ejecutamos perror("execvp") y luego exit(1).
execvp(cmds[i][0], cmds[i]);
// cmds[i][0] → Nombre del comando (ejemplo: "ls").
// cmds[i] → Lista de argumentos ({"ls", "-l", NULL}).
execvp remplaza el proceso actual cmds[i] con el nuevo comando cmds[i][0]

wait(NULL); hace que el proceso padre espere a que termine un proceso hijo.
Si hay varios hijos, wait(NULL); solo espera a uno, por eso el loop

// obviamente aqui un tester.sh va a ir mal porque bash hace las
// cosas de forma diferente que picoshell. Chatgpt dice que son los mismos
// test no me he parado a comprobar.