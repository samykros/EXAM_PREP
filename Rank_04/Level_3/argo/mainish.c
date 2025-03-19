
// #include "argo.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char *key;
	union
	{
		char *string_val;
		long number_val;
		int bool_val; // 0 or 1 for boolean
		void *null_val;
		struct json *nested_val; // for nested objects/arrays
	};
	int type; // Type identifier (e.g., JSON_STRING, JSON_NUMBER, etc.)
} json;

#define JSON_STRING 1
#define JSON_NUMBER 2
#define JSON_BOOLEAN 3
#define JSON_NULL 4
#define JSON_OBJECT 5
#define JSON_ARRAY 6

// Function to read a string and handle escape characters
void read_string(FILE *stream, char *buffer, int *i)
{
	int next_c;
	while ((next_c = fgetc(stream)) != EOF)
	{
		if (next_c == '"')
			break; // End of string
		if (next_c == '\\')
		{ // Handle escape characters
			next_c = fgetc(stream);
			if (next_c == 'n')
				buffer[(*i)++] = '\n';
			else if (next_c == 't')
				buffer[(*i)++] = '\t';
			else if (next_c == '\"')
				buffer[(*i)++] = '\"';
			else if (next_c == '\\')
				buffer[(*i)++] = '\\';
			else
				buffer[(*i)++] = next_c; // Invalid escape
		}
		else
		{
			buffer[(*i)++] = next_c;
		}
	}
	buffer[*i] = '\0';
}

// Function to read a number
void read_number(FILE *stream, char *buffer, int *i)
{
	int next_c;
	buffer[(*i)++] = fgetc(stream); // Capture the first digit or '-'
	while (isdigit(next_c = fgetc(stream)) || next_c == '.' || next_c == 'e' || next_c == 'E' || next_c == '+' || next_c == '-')
	{
		buffer[(*i)++] = next_c;
	}
	ungetc(next_c, stream); // Push back the last character
	buffer[*i] = '\0';
}

// Function to read a boolean
int read_boolean(FILE *stream, char *buffer, int *i)
{
	int next_c;
	buffer[(*i)++] = fgetc(stream); // Capture the first character
	while (i < 5 && (next_c = fgetc(stream)) != EOF)
	{
		buffer[(*i)++] = next_c;
	}
	buffer[*i] = '\0';
	if (strcmp(buffer, "true") == 0)
		return 1;
	if (strcmp(buffer, "false") == 0)
		return 0;
	return -1; // Unexpected token
}

// Function to read null
int read_null(FILE *stream, char *buffer, int *i)
{
	int next_c;
	buffer[(*i)++] = fgetc(stream); // Capture the first character
	while (i < 4 && (next_c = fgetc(stream)) != EOF)
	{
		buffer[(*i)++] = next_c;
	}
	buffer[*i] = '\0';
	if (strcmp(buffer, "null") == 0)
		return 0; // Null
	return -1;    // Unexpected token
}

int argo(json *dst, FILE *stream)
{
	int c = 0, next_c = 0;
	json *current = dst; // This will be the root JSON object
	json *stack[100];    // To manage nested structures (limited size for simplicity)
	int stack_top = -1;

	while ((c = fgetc(stream)) != EOF)
	{
		// Skip whitespace
		if (isspace(c))
			continue;

		if (c == '{')
		{
			// JSON object start
			stack[++stack_top] = current;
			current = malloc(sizeof(json));
			current->type = JSON_OBJECT;
			current->key = NULL;
			current->nested_val = NULL;
		}
		else if (c == '}')
		{
			// JSON object end
			current = stack[stack_top--];
		}
		else if (c == '[')
		{
			// JSON array start
			stack[++stack_top] = current;
			current = malloc(sizeof(json));
			current->type = JSON_ARRAY;
			current->key = NULL;
			current->nested_val = NULL;
		}
		else if (c == ']')
		{
			// JSON array end
			current = stack[stack_top--];
		}
		else if (c == '"')
		{
			// JSON string
			char buffer[256];
			int i = 0;
			read_string(stream, buffer, &i);
			current->type = JSON_STRING;
			current->string_val = strdup(buffer);
		}
		else if (isdigit(c) || c == '-')
		{
			// JSON number
			char buffer[256];
			int i = 0;
			read_number(stream, buffer, &i);
			current->type = JSON_NUMBER;
			current->number_val = strtol(buffer, NULL, 10); // Convert to long
		}
		else if (c == 't' || c == 'f')
		{
			// JSON boolean
			char buffer[6]; // "true\0" or "false\0"
			int i = 0;
			int boolean_val = read_boolean(stream, buffer, &i);
			if (boolean_val >= 0)
			{
				current->type = JSON_BOOLEAN;
				current->bool_val = boolean_val;
			}
			else
			{
				fprintf(stderr, "Unexpected token '%c'\n", c);
				return -1;
			}
		}
		else if (c == 'n')
		{
			// JSON null
			char buffer[5]; // "null\0"
			int i = 0;
			int null_val = read_null(stream, buffer, &i);
			if (null_val == 0)
			{
				current->type = JSON_NULL;
				current->null_val = NULL;
			}
			else
			{
				fprintf(stderr, "Unexpected token '%c'\n", c);
				return -1;
			}
		}
		else
		{
			fprintf(stderr, "Unexpected token '%c'\n", c);
			return -1;
		}
	}

	if (c == EOF && current != dst)
	{
		fprintf(stderr, "Unexpected end of input\n");
		return -1;
	}

	return 1; // Success
}
