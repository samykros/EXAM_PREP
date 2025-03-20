#include "vbc.h"

t_node *create_operator_node(char op, t_node *left, t_node *right)
{
	t_node *node = malloc(sizeof(t_node));

	node->op = op;
	node->value = '\0';
	node->left = left;
	node->right = right;

	return node;
}

t_node *create_number_node(int value)
{
	t_node *node = malloc(sizeof(t_node));

	node->op = '\0';
	node->value = value;
	node->left = NULL;
	node->right = NULL;

	return node;
}

t_node *parse_factor(char **expr)
{
	if (**expr == '(')
	{
		(*expr)++;
		t_node *node = parse_expression(expr);
		(*expr)++;
	}

	return create_number_node(*(*expr)++ - '0');
}

t_node *parse_term(char **expr)
{
	t_node *node = parse_factor(expr);

	while (**expr == '*')
	{
		char op = *(*expr)++;
		node = create_operator_node(op, node, parse_factor(expr));
	}

	return node;
}

t_node *parse_expression(char **expr)
{
	t_node *node = parse_term(expr);

	while (**expr == '+')
	{
		char op = *(*expr)++;
		node = create_operator_node(op, node, parse_term(expr));
	}

	return node;
}

// char op = *(*expr)++;	Guarda el carácter actual y luego avanza el puntero.
// (*expr)++;	Solo avanza el puntero, sin guardar el valor actual.

int evaluate(t_node *node)
{
	if (node->op == '\0')
		return node->value;

	int left = evaluate(node->left);
	int right = evaluate(node->right);

	if (node->op == '+')
		return left + right;
	if (node->op == '*')
		return left * right;

	return 0;
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		char *expr = argv[1];
		t_node *ast = parse_expression(&expr);
		printf("%d\n", evaluate(ast));
	}
}