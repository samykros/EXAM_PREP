#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Node {
    char op;
    int value;
    struct Node *left;
    struct Node *right;
} Node;

void error(const char *msg, char c) {
    fprintf(stderr, "%s: %c\n", msg, c);
    exit(1);
}

void freeAST(Node *node) {
    if (node) {
        freeAST(node->left);
        freeAST(node->right);
        free(node);
    }
}

void skipWhitespace(const char **input) {
    while (isspace(**input)) {
        (*input)++;
    }
}

Node *parseExpression(const char **input);
Node *parseTerm(const char **input);
Node *parseFactor(const char **input);

Node *parseExpression(const char **input) {
    Node *left = parseTerm(input);
    if (!left)
        return NULL;

    while (**input == '+') {
        char op = **input;
        (*input)++;
        Node *right = parseTerm(input);
        if (!right) {
            freeAST(left);
            return NULL;
        }
        Node *node = malloc(sizeof(Node));
        node->op = op;
        node->left = left;
        node->right = right;
        left = node;
    }
	if (**input != '\0' || **input != ')') {
		error("Unexpected character", **input);
		freeAST(left);
		return NULL;
	}

    return left;
}

Node *parseTerm(const char **input) {
    Node *left = parseFactor(input);
    if (!left)
        return NULL;

    while (**input == '*') {
        char op = **input;
        (*input)++;
        Node *right = parseFactor(input);
        if (!right) {
            freeAST(left);
            return NULL;
        }
        Node *node = malloc(sizeof(Node));
        node->op = op;
        node->left = left;
        node->right = right;
        left = node;
    }

    return left;
}

Node *parseFactor(const char **input) {
    skipWhitespace(input);

    if (**input == '(') {
        (*input)++;
        Node *node = parseExpression(input);

        skipWhitespace(input);
        if (**input != ')') {
            if (**input != '\0')
                error("Unbalanced parentheses", **input);
            else
                error("Reached end of line", '\0');
            freeAST(node);
            return NULL;
        }
        (*input)++; // Consume ')'
        return node;
    } else if (isdigit(**input)) {
        Node *node = malloc(sizeof(Node));
        node->op = '\0';
        node->value = **input - '0';
        node->left = node->right = NULL;
        (*input)++;
        return node;
    } else {
        error("Unexpected character", **input);
    }
    return NULL; // To satisfy the compiler
}

// Evaluate the AST
int evaluateAST(Node *node) {
    if (!node->op) {
        return node->value;
    }

    int leftVal = evaluateAST(node->left);
    int rightVal = evaluateAST(node->right);

    if (node->op == '+') {
        return leftVal + rightVal;
    } else if (node->op == '*') {
        return leftVal * rightVal;
    }

    return 0; // Should never reach here
}

int main() {
    char input[256];
    Node *ast;
    int result;

    while (1) {
        printf("Enter an arithmetic expression (or type 'exit' to quit): ");

        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input.\n");
            break;
        }

        // Remove the newline character at the end of input
        input[strcspn(input, "\n")] = 0;

        // Check if the user wants to exit
        if (strcmp(input, "exit") == 0) {
            break;
        }

        const char *ptr = input;
        ast = parseExpression(&ptr);

        // Check for leftover characters after parsing
        skipWhitespace(&ptr);
        if (*ptr != '\0' && *ptr != '\n') {
            error("Unexpected character", *ptr);
            freeAST(ast);
            continue;
        }

        // Evaluate and print the result
        if (ast) {
            result = evaluateAST(ast);
            printf("Result: %d\n", result);
        }
        // Clean up
        freeAST(ast);
    }

    printf("Goodbye!\n");
    return 0;
}
