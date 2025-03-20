#include <unistd.h>


int main()
{
	write(STDOUT_FILENO, "Hola mundo!\n", 12);
	return 0;
}