#include "vbc.h"

t_node *create_number_node(int value)
{
	t_node *node = malloc(sizeof(t_node));

	node->op = '\0'; // Indica que es un número
	node->value = value;
	node->left = NULL;
	node->right = NULL;

	return node;
}

t_node *create_operator_node(char op, t_node *left, t_node *right)
{
	t_node *node = malloc(sizeof(t_node));

	node->op = op; // '+' o '*'
	node->value = 0; // No se usa
	node->left = left;
	node->right = right;

	return node;
}

int evaluate(t_node *node)
{
	if (node->op == '\0')
		return node->value; // Si es número, devolverlo

	int left = evaluate(node->left); // jerarquia de op de izquierda
	int right = evaluate(node->right); // a derecha
		
	if (node->op == '+') // El orden dentro de evaluate() de + y * no importa,
		return left + right; // porque ya respetamos la jerarquía en el parsing.
	if (node->op == '*')
		return left * right;
		
	return 0; // Nunca debería llegar aquí
}

t_node *parse_factor(char **expr)
{
	if (**expr == '(') // Si encontramos un paréntesis
	{
		(*expr)++; // Saltar paréntesis
		t_node *node = parse_expression(expr); // Procesar lo que hay dentro del paréntesis
		(*expr)++; // Saltamos el ')'
		return node;
	}

	return create_number_node(*(*expr)++ - '0'); // Convertir '3' a 3 y avanzar puntero para lo siguiente
}
// Te recuerdo: '0' = 48, '1' = 49, '3' = 51
// '3' - '0' = 51 - 48 = 3

t_node *parse_term(char **expr)
{
	t_node *node = parse_factor(expr);

	while (**expr == '*')
	{
		char op = *(*expr)++;
		node = create_operator_node(op, node, parse_factor(expr)); // volver a parse_factor por importancia
	}

	return node;
}

t_node *parse_expression(char **expr)
{
	t_node *node = parse_term(expr);

	while (**expr == '+') // ** lo que hay en expr guardado
	{
		char op = *(*expr)++; // es la direccion + avanzar puntero char *(*expr)
		node = create_operator_node(op, node, parse_term(expr));
	}

	return node;
}

void free_ast(t_node *node)
{
	if (!node)
		return;  // Si el nodo es NULL, no hacemos nada

	free_ast(node->left);   // Liberamos subárbol izquierdo
	free_ast(node->right);  // Liberamos subárbol derecho

	free(node);  // Liberamos nodo actual
}


/////////////////////////////////
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////


int main(int argc, char **argv)
{
	if (argc == 2)
	{
		char *expr = argv[1];
		t_node *ast = parse_expression(&expr);
		printf("%d\n", evaluate(ast));
		free(ast);
	}

	return 0;
}


// Diferencias entre *(*expr)++, **expr++, y (**expr++) (piensa en jerarquia de operaciones ())
/*
📌 Paso a paso:
1️⃣ *expr obtiene el primer carácter de la cadena ('3').
2️⃣ (*expr)++ avanza el puntero expr al siguiente carácter ('+').
3️⃣ *(*expr)++ primero obtiene '3' y luego avanza expr a '+'.

📌 Estado de memoria antes y después:

Antes: expr → "3+4"
           ↑
Después: expr → "3+4"
                ↑

📌 Resultado:

    c = '3'
    expr ahora apunta a '+'.

✅ Este es el correcto si queremos leer el carácter actual y avanzar el puntero.
*/

/*
void run_test(char *expr, int expected)
{
	char *exp_ptr = expr;
	t_node *ast = parse_expression(&exp_ptr);
		
	if (!ast) {
		printf("🔹 Test: \"%s\" -> ❌ No AST generado\n", expr);
		return;
	}

	int result = evaluate(ast);
		
	if (result == expected)
		printf("✅ Test PASÓ: \"%s\" = %d\n", expr, result);
	else
		printf("❌ Test FALLÓ: \"%s\" -> Esperado %d, Obtenido %d\n", expr, expected, result);
}

int main()
{
	printf("🔹 Ejecutando tests para vbc...\n\n");

	// ✅ Casos básicos
	run_test("2+3", 5);
	run_test("4*5", 20);
	run_test("7+2*3", 13);  // Multiplicación tiene más prioridad
	run_test("2*3+4", 10);
	run_test("2*(3+4)", 14);
	run_test("(2+3)*4", 20);
		
	// ✅ Casos con varios niveles de paréntesis
	run_test("(1+(2*3))", 7);
	run_test("((1+2)*(3+4))", 21);
	run_test("((2+3)*(4*(1+1)))", 40);
		
	// ✅ Casos límite
	run_test("3", 3);  // Un solo número
	run_test("(3)", 3);  // Un solo número con paréntesis
	run_test("(((((3)))))", 3);  // Paréntesis anidados sin cambios
		
	// ✅ Edge cases de parsing? No tengo en cuenta cadenas erroneas
	run_test("", 0);  // Expresión vacía (según lo que decidiste, no imprime nada)
	run_test("()", 0);  // Paréntesis vacíos
	run_test("hello", 0);  // Strings

	return 0;
}*/