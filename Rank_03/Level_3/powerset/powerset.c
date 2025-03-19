/* 
Assignment name: powerset
Expected files: //unknown
Allowed functions: atoi, printf, malloc, calloc, realloc, free

args:	n[s]
	[s] - set of numbers
	[n] - integer value

task: 
display subsets of [s] whose sum is equal to n. No duplicates

malloc error - exit with code 1
if there is no set satisfying the task print new line
empty set is also sub-set!
set like in python
positive 1 negative (??)

We will not test with invalid set
*/

#include <stdio.h>
#include <stdlib.h>

void result(int *set, int size, int *subset, int subset_size, int n, int *sum)
{
	// Base case: check if the current subset sums to n
	if (*sum == n)
	{
		for (int i = 0; i < subset_size; i++)
		{
			printf("%d", subset[i]);
			if (i < subset_size - 1)
				printf(" ");
		}
		printf("\n");
	}

	for (int i = 0; i < size; i++)
	{
		// Create a new subset including the current element
		subset[subset_size] = set[i];
		*sum = *sum + set[i];

		// Recursive call to find subsets with the remaining elements
		result(set + i + 1, size - i - 1, subset, subset_size + 1, n, sum);

		// Restart
		*sum = *sum - set[i];
	}
}

int main(int argc, char **argv)
{
	if (argc < 3)
		return 1;

	int n = atoi(argv[1]);
	int size = argc - 2;
	int *set = malloc(size * sizeof(int));
	if (!set)
		return 1;

	for (int i = 0; i < size; i++)
		set[i] = atoi(argv[i + 2]); // + 2 porque 0 es a.out y 1 es n

	int *subset = malloc(size * sizeof(int));
	if (!subset)
	{
		free(set);
		return 1;
	}

	int sum = 0;
	result(set, size, subset, 0, n, &sum);

	// If no subsets were found, print a new line
	if (sum == 0)
		printf("\n");

	free(set);
	free(subset);
	return 0;
}
