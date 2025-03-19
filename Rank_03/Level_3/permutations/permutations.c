/* 
Assignment name: permutations
Expected files: *.c *.h
Function allowed: puts, malloc, calloc, realloc, free, write

Write a program that will print all the permutations of a string given as an argument.
Solutions must be given in an alphabetical order.

We will not try your program with duplicates. example: "abccd"

Examples:
./permutations a
a

./permutations ab
ab
ba

./permutations abc
abc
acb
bac
bca
cab
cba 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
permutate("abc", 1)
	permutate("abc", 2)
	permutate("acb", 2)
permutate("bac", 1)
	permutate("bac", 2)
	permutate("bca", 2)
permutate("cab", 1)
	permutate("cab", 2)
	permutate("cba", 2)
*/

/*
void	sort_string(char *str)
{
	int	c = 1;
	int	tmp;
	int	size = strlen(str);

	while (c < size)
	{
		if (str[c] < str[c - 1])
		{
			tmp = str[c];
			str[c] = str[c - 1];
			str[c - 1] = tmp;
			c = 0;
		}
		c++;
	}
}
*/

void sort_string(char *str)
{
	int len = strlen(str);
	int swapped = 1; // Inicializamos en 1 para entrar al while

	while (swapped)
	{
		swapped = 0;
		for (int i = 0; i < len - 1; i++)
		{
			if (str[i] > str[i + 1])
			{
				char tmp = str[i];
				str[i] = str[i + 1];
				str[i + 1] = tmp;
				swapped = 1; // Si hubo un cambio, seguimos iterando
			}
		}
	}
}

void permutate(char *str, int pos, int len)
{
	char	old;
	char	*tmp;

	if (pos == len - 1)
	{
		puts(str);
		return ;
	}
	for (int i = pos; i < len; i++)
	{
		tmp = strdup(str);
		old = tmp[pos];
		tmp[pos] = tmp[i];
		tmp[i] = old;
		sort_string(tmp + pos + 1);
		permutate(tmp, pos + 1, len);
		free(tmp);
	}
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		sort_string(argv[1]);
		permutate(argv[1], 0, strlen(argv[1]));
	}
	return 0;
}
