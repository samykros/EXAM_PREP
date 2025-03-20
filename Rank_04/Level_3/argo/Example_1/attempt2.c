#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum defining types of JSON values

void ft_error(const char *message, char unexpected)
{
	if (unexpected != '\0')
		printf("%s: '%c'\n", message, unexpected);
	else
		printf("%s\n", message);
}
// JSON structure
typedef struct json{
    enum{
		JSON_STRING,
		JSON_INTEGER,
		JSON_MAP
	} type;
    union {
        char *str;
        int integer;
        struct {
			struct pair *data;
			int size;
		}map;
    };
} json;

// Pair structure for key-value pairs
typedef struct pair {
    char *key;
    json *value;
} pair;

// Function to allocate memory and handle errors
void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Function to parse a JSON string
char *parse_string(FILE *stream, char **dest) {
    int c, len = 0, capacity = 16;
    char *buffer = xmalloc(capacity);

    while ((c = fgetc(stream)) != EOF) {
        if (c == '\\') { // Handle escape characters
            c = fgetc(stream);
            if (c == EOF) break;
            if (c == 'n') c = '\n';
            else if (c == 't') c = '\t';
            else if (c == 'r') c = '\r';
        } else if (c == '"') { // End of string
            buffer[len] = '\0';
            *dest = buffer;
            return buffer;
        }

        if (len + 1 >= capacity) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (!buffer) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        buffer[len++] = c;
    }
    free(buffer);
    return NULL; // Error: unexpected end of input
}

// Function to parse integers
int parse_integer(FILE *stream) {
    int num = 0, c, sign = 1;

    if ((c = fgetc(stream)) == '-') { // Handle negative integers
        sign = -1;
    } else {
        ungetc(c, stream);
    }

    while ((c = fgetc(stream)) != EOF && isdigit(c)) {
        num = num * 10 + (c - '0');
    }
    if (c != EOF) ungetc(c, stream); // Put back the non-digit
    return num * sign;
}

int argo(json *dst, FILE *stream);

// Function to parse JSON objects (maps)
struct pair *parse_map(FILE *stream, int *size) {
    int c, count = 0, capacity = 16;
    pair *pairs = xmalloc(capacity * sizeof(pair));

    while ((c = fgetc(stream)) != EOF) {
        if (isspace(c)) continue;
        if (c == '}') break;
        if (c != '"') {
            ft_error("Expected '\"' in JSON map", c);
            free(pairs);
            return NULL;
        }

        char *key;
        if (!parse_string(stream, &key)) {
            fprintf(stderr, "Error parsing JSON string\n");
            free(pairs);
            return NULL;
        }

        while ((c = fgetc(stream)) != EOF && isspace(c));
        if (c != ':') {
            fprintf(stderr, "Expected ':' in JSON map\n");
            free(pairs);
            return NULL;
        }

        json *value = xmalloc(sizeof(json));

        if ((c = fgetc(stream)) == '"') {
            value->type = JSON_STRING;
            parse_string(stream, &value->str);
        } else if (isdigit(c) || c == '-') {
            ungetc(c, stream);
            value->type = JSON_INTEGER;
            value->integer = parse_integer(stream);
        } else if (c == '{') {
            value->type = JSON_MAP;
            value->map.data = parse_map(stream, &value->map.size);
            if (!value->map.data) {
                fprintf(stderr, "Error parsing JSON map\n");
                free(value);
                free(pairs);
                return NULL;
            }
        } else {
            fprintf(stderr, "Unexpected token '%c'\n", c);
            free(value);
            free(pairs);
            return NULL;
        }

        // Add the key-value pair to the map
        pairs[count].key = key;
        pairs[count].value = value;
        count++;

        // Resize the pairs array if needed
        if (count >= capacity) {
            capacity *= 2;
            pairs = realloc(pairs, capacity * sizeof(pair));
            if (!pairs) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }

        // Check for the next character (',' or '}')
        while ((c = fgetc(stream)) != EOF && isspace(c));
        if (c == ',') continue;
        if (c == '}') break;

        fprintf(stderr, "Expected ',' or '}' in JSON map\n");
        free(pairs);
        return NULL;
    }

    *size = count;
    return pairs;
}

int argo(json *dst, FILE *stream){
	int c;
	while ((c = fgetc(stream)) != EOF) {
		if (isspace(c)) continue;
		if (c == '{') {
			dst->type = JSON_MAP;
			dst->map.data = parse_map(stream, &dst->map.size);
			if (!dst->map.data) {
				fprintf(stderr, "Error parsing JSON map\n");
				return -1;
			}
			return 1;
		} else if (c == '"') {
			dst->type = JSON_STRING;
			parse_string(stream, &dst->str);
			return 1;
		} else if (isdigit(c) || c == '-') {
			ungetc(c, stream);
			dst->type = JSON_INTEGER;
			dst->integer = parse_integer(stream);
			return 1;
		} else {
			fprintf(stderr, "Unexpected token '%c'\n", c);
			return -1;
		}
	}
	fprintf(stderr, "Unexpected end of input\n");
	return -1;
}


// Print JSON recursively
void print_json(const struct json *json, int depth) {
    switch (json->type) {
        case JSON_STRING:
            printf("\"%s\"", json->str);
            break;
        case JSON_INTEGER:
            printf("%d", json->integer);
            break;
        case JSON_MAP: {
            printf("{");
            for (int i = 0; i < json->map.size; i++) {
                if (i > 0) {
                    printf(", ");
                }
                printf("\"%s\": ", json->map.data[i].key);
                print_json(json->map.data[i].value, depth + 1);
            }
            printf("}");
            break;
        }
        default:
            fprintf(stderr, "Unknown JSON type\n");
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    struct json root;
    int result;

    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Error opening file");
            return EXIT_FAILURE;
        }
    }

    result = argo(&root, input);
    if (result == 1) {
        print_json(&root, 0);  // Print the JSON structure
        printf("$\n");
    } else {
        // Handle parsing error gracefully
        fprintf(stderr, "Parsing error occurred.\n");
        return EXIT_FAILURE;
    }

    fclose(input);
    return EXIT_SUCCESS;
}
