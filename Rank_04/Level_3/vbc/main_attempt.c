#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
	char op;   // Operator (+, *)
	int value; // Only used if node is a number
	struct Node *left;
	struct Node *right;
} Node;

// Skip whitespace in the input
void skipWhitespace(const char **input)
{
	while (**input == ' ')
		(*input)++;
}

// Print an error message and exit
void error(const char *message, char unexpected)
{
	if (unexpected != '\0')
		printf("%s: '%c'\n", message, unexpected);
	else
		printf("%s\n", message);
}
// Free memory used by the AST
void freeAST(Node *node)
{
	if (!node)
		return;
	freeAST(node->left);
	freeAST(node->right);
	free(node);
}

Node *parseExpression(const char **input);

// Parse a term (handles *)
Node *parseOperator(const char **input)
{
	Node *left = parseExpression(input);
	if (!left)
		return NULL;
	char op = **input;
	if(op == '*' || op == '+'){
		(*input)++;
		Node *right= parseExpression(input);
		if(!right){
			freeAST(left);
			return NULL;
		}
		Node *node = malloc(sizeof(Node));
		node->op = op;
		node->left = left;
		node->right = right;
		return node;
	}
	return left;
}

Node *parseExpression(const char **input)
{
	if(**input == '('){
		(*input)++;
		Node *node = parseOperator(input);
		if (**input != ')'){
			return NULL;
		}
		(*input)++;
		return node;

	} else if (isdigit(**input)){
		Node *node = malloc(sizeof(Node));
		node->op = '\0';
		node->value = **input - '0';
		node->right = node->left = NULL;
		(*input)++;
		return node;
	}else{
		error("unexpected", **input);
	}
	
}
// Evaluate the AST
int evaluateAST(Node *node)
{
	if (!node->op)
	{
		return node->value;
	}

	int leftVal = evaluateAST(node->left);
	int rightVal = evaluateAST(node->right);

	if (node->op == '+')
	{
		return leftVal + rightVal;
	}
	else if (node->op == '*')
	{
		return leftVal * rightVal;
	}

	return 0; // Should never reach here
}

int main()
{
	char input[256];
	Node *ast;
	int result;

	while (1)
	{
		printf("Enter an arithmetic expression (or type 'exit' to quit): ");

		if (!fgets(input, sizeof(input), stdin))
		{
			printf("Error reading input.\n");
			break;
		}

		// Remove the newline character at the end of input
		input[strcspn(input, "\n")] = 0;

		// Check if the user wants to exit
		if (strcmp(input, "exit") == 0)
		{
			break;
		}

		const char *ptr = input;
		ast = parseOperator(&ptr);

		// Check for leftover characters after parsing
		skipWhitespace(&ptr);
		if (*ptr != '\0' && *ptr != '\n')
		{
			error("Unexpected character", *ptr);
			freeAST(ast);
			continue;
		}

		// Evaluate and print the result
		if (ast){
			result = evaluateAST(ast);
			printf("Result: %d\n", result);
		}
		// Clean up
		freeAST(ast);
	}

	printf("Goodbye!\n");
	return 0;
}
