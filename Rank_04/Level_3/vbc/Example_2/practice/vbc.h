#include <stdio.h>
#include <stdlib.h>

typedef struct s_node {
	char op;
	int value;
	struct s_node *left;
	struct s_node *right;
} t_node;

t_node		*create_number_node(int value);
t_node		*create_operator_node(int op, t_node *left, t_node *right);
int			evaluate(t_node *node);
t_node		*parse_term(char **expr);
t_node		*parse_expression(char **expr);
t_node		*parse_factor(char **expr);
void		free_nodes(t_node *node);
