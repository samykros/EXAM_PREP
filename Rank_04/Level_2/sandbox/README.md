Creamos un clon del mundo (fork()) para que Función juegue sin romper nada en nuestra casa.
Le damos un cronómetro (alarm()) para que no se quede jugando para siempre.
Esperamos a ver qué hace (waitpid()) y analizamos su comportamiento.
Si se porta bien (exit(0)), decimos "¡Buen chico!".
Si se porta mal (exit(1), segfault, abort...), lo regañamos con un mensaje adecuado.
Si se queda jugando demasiado tiempo, lo sacamos a la fuerza (kill()).

	Si pid == 0 → Somos el clon (el hijo). Aquí ejecutamos la función f().
	Si pid > 0 → Somos el padre. Esperamos al hijo y vemos qué hace.
	Si pid < 0 → Oh no, no pudimos hacer el clon (error).

🔹	En palabras simples: fork() crea dos versiones del programa.

Piensa en status como una caja cerrada con pistas sobre cómo murió el proceso hijo.
Las macros son como rayos X 🔍 que nos permiten ver dentro de la caja.

MACROS:

Si el proceso es un coche 🚗 y status es un informe de accidente:
	WIFEXITED(status) == true → 🚗💨 El coche llegó al destino (exitó normalmente).
	WIFEXITED(status) == false → 🚗🔥💥 El coche explotó (se mató con una señal).

Si status es un informe de notas de examen 🎓:
	WEXITSTATUS(status) == 0 → 🏆 Aprobó con 10/10.
	WEXITSTATUS(status) == 1 → 📉 Suspendió con 3/10.
	WEXITSTATUS(status) == 2 → 📉 Fracasó completamente.

WIFSIGNALED(status) == true → 🚶💥🔫 El hijo fue asesinado.
WIFSIGNALED(status) == false → 🚶🛏 Se fue a dormir normalmente (exit(n)).

WTERMSIG(status) == SIGSEGV → 🧠💥 El proceso tocó memoria prohibida y explotó.
WTERMSIG(status) == SIGKILL → 🔪💥 El proceso fue eliminado con kill -9.
WTERMSIG(status) == SIGABRT → 🚨💥 El proceso se autodestruyó con abort().