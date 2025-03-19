/*Expected files : *.c *.h
Allowed  functions: atoi, printf, write

Write a program that will print all the solutions to the n_queens problem for a n given as an argument.
We will not test with negative values. The order of the solutions is not important 
You will display the solutions under following format 
<p1> <p2> <p3>...\n
Where Pn is the line index of the queen in each column starting from 0

Examples:
./n_queens 2 ->do nothing

./n_queen 4 -> 1 3 0 2
				2 0 3 1
*/

#include <stdio.h>
#include <stdlib.h>

int issafe(int board[50], int row, int col)
{
	// Comprobar la columna
	for (int i = 0; i < row; i++)
	{
		if (board[i] == col || board[i] - i == col - row || board[i] + i == col + row)
			return 0; // Hay una reina en la misma columna o diagonal
	}

	return 1;
}

void put_queen(int board[50], int row, int size)
{
	if (row == size)
	{
		for (int i = 0; i < size; i++)
			printf("%d ", board[i]);
		printf("\n");
		return;
	}

	// Intentar colocar una reina en cada columna de la fila actual
	for (int col = 0; col < size; col++)
	{
		if (issafe(board, row, col))
		{
			board[row] = col; // Colocar la reina
			put_queen(board, row + 1, size); // Llamar recursivamente para la siguiente fila
			board[row] = -1; // Retroceder (backtrack)
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		int size = atoi(argv[1]);
		int board[50];

		put_queen(board, 0, size);
	}

	return 0;
}
