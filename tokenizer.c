#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TOKENS (1000000)
#define MAX_LINE_LENGTH (1000000)
#define EXIT_SUCCESS 0

char *tokens[MAX_TOKENS];
int num_tokens = 0;
char *delimiters = " ";

void add_spaces_around_chars(char *str, const char *chars)
{
  // Create a copy of the input string
  char temp[strlen(str) + 1];
  strcpy(temp, str);

  // Initialize variables
  char *out = str;

  // Loop through all characters in the string
  for (int i = 0; i < strlen(temp); i++)
  {
    // Check if the current character is in the set of characters
    if (strchr(chars, temp[i]))
    {
      // Add a space before the character
      *out++ = ' ';

      // Copy the character to the output string
      *out++ = temp[i];

      // Add a space after the character
      *out++ = ' ';
    }
    else
    {
      // Copy the character to the output string
      *out++ = temp[i];
    }
  }

  // Terminate the output string
  *out = '\0';
}

int tokenize(char *input, char *tokens[])
{
  int num_tokens = 0;

  char *token = strtok(input, delimiters);

  while (token != NULL && num_tokens < MAX_TOKENS)
  {
    tokens[num_tokens++] = token;
    token = strtok(NULL, delimiters);
  }

  return num_tokens;
}

void tokenizer(char *filename)
{
  FILE *in_file = fopen(filename, "r");
  char line[MAX_TOKENS];
  char str[MAX_TOKENS];
  int j = 0;
  while (fgets(line, MAX_LINE_LENGTH, in_file))
  {
    for (int i = 0; i < strlen(line); ++i)
    {
      if (line[i] == '\t' || line[i] == '\n' || line[i] == '\r')
        continue;
      str[j] = line[i];
      j++;
    }
  }
  add_spaces_around_chars(str, ",;=>+-*/(){}");
  num_tokens = tokenize(str, tokens);
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
    exit(EXIT_FAILURE);
  }
  tokenizer(argv[1]);
  for (int i = 0; i < num_tokens; i++)
  {
    printf("Token %d: %s\n", i + 1, tokens[i]);
  }
  return 0;
}
