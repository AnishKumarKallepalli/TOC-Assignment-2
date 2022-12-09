#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TOKENS (1000000)
#define MAX_LINE_LENGTH (1000000)
#define EXIT_SUCCESS 0

char *tokens[MAX_TOKENS];
int num_tokens = 0;
int line_position = 0;
char str[10000];

void tokenize(char *filename)
{

    FILE *in_file = fopen(filename, "r");

    char line[MAX_TOKENS];
    while (fgets(line, MAX_LINE_LENGTH, in_file))
    {
        for (int i = 0; i < strlen(line); ++i)
        {
            if (line[i] == '\t' || line[i] == '\n' || line[i] == '\r')
                continue;
            if (line[i] == ' ' || line[i] == ';' || line[i] == ',' || line[i] == '=' || line[i] == '>' || line[i] == '+' || line[i] == '-' || line[i] == '/' || line[i] == '*' || line[i] == '(' || line[i] == ')' || line[i] == '{' || line[i] == '}')
            {
                if (strlen(str))
                {
                    line_position = 0;
                    tokens[num_tokens] = (char*)malloc(sizeof(str));

                    strcpy(tokens[num_tokens++], str);
                    memset(str, '\0', sizeof(str));
                }

                if (line[i] == ' ')
                    continue;

                tokens[num_tokens] = (char*)malloc(2);
                tokens[num_tokens++][0] = line[i];
            }
            else
            {
                str[line_position++] = line[i];
            }
        }
    }
}

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
        exit(EXIT_FAILURE);
    }
    tokenize(argv[1]);
    for (int i = 0; i < num_tokens; ++i)
    {
        printf("%s\n", tokens[i]);
    }
    // printf("\n");
    // printf("%d\n", num_tokens);
    return EXIT_SUCCESS;
}