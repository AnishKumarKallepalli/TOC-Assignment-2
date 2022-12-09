#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKENS (1000000)
#define MAX_LINE_LENGTH (1000000)
#define MAX_VARIABLES (100)
#define EXIT_SUCCESS 0

char *tokens[MAX_TOKENS];
int num_tokens = 0;
char *delimiters = " ";
char tree[MAX_TOKENS];
int tree_index = 0;

char keywords[4][10]= {"for", "read", "write", "int"};

typedef struct{
    char name[20];
    int value;
} var;

var variables[MAX_VARIABLES];
int variable_count = 0;


void parse_program(int);
void parse_declaration(int);
void parse_multiple_variables(int);
void parse_variable(int);
void parse_statement(int);
void parse_single_statement(int);
void parse_expression(int);
void parse_assignment(int);
void parse_read(int);
void parse_write(int);
void parse_for_loop(int);

void (*parser_functions[4])(int index) = {parse_for_loop, parse_read, parse_write, parse_assignment};
// void (*parser_functions[5])(int index);


void add_spaces_around_chars(char *str, const char *chars){
  // Create a copy of the input string
  char temp[strlen(str) + 1];
  strcpy(temp, str);

  // Initialize variables
  char *out = str;

  int len = strlen(temp);

  // Loop through all characters in the string
  for (int i = 0; i < len; i++)
  {
    if(i < len - 1 && temp[i] == '=' && temp[i + 1] == '='){
      *out++ = temp[i++];
      *out++ = temp[i];
    }
    // Check if the current character is in the set of characters
    else if (strchr(chars, temp[i]))
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

int tokenize(char *input, char *tokens[]){
  int num_tokens = 0;

  char *token = strtok(input, delimiters);

  while (token != NULL && num_tokens < MAX_TOKENS)
  {
    tokens[num_tokens++] = token;
    token = strtok(NULL, delimiters);
  }

  return num_tokens;
}

void tokenizer(char *filename){
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

int is_keyword(char *str){

    for(int i = 0; i < 4; i++){
        if(!strcmp(str, keywords[i]))
            return i;
    }

    return -1;
}

int is_variable(char *str){

    for(int i = 0; i < variable_count; i++){
        if(!strcmp(variables[i].name, str))
            return i;
    }

    return -1;
}

bool valid_indentifier(char *identifier){

    for(int i = 0; identifier[i] != 0; i++){
        if(!isalpha(identifier[i]))
            return false;
    }

    if(is_keyword(identifier) >= 0 || is_variable(identifier) >= 0)
        return false;

    return true;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
    exit(EXIT_FAILURE);
  }
  tokenizer(argv[1]);

  parse_program(0);

  printf("%s\n", tree);
  return 0;
}

void parse_program(int index){

    strcpy(tree + tree_index, "[P");
    tree_index += 2;

    if(strcmp(tokens[index], "int") == 0){
        parse_declaration(index);
    }

    while (tokens[index++][0] != ';') ;

    if(index == num_tokens){
        tree[tree_index++] = ']';
        return;
    }

    //Uncomment when complete
    // parse_statement(index);

    tree[tree_index++] = ']';
}

void parse_declaration(int index){

    strcpy(tree + tree_index, "[D[int]");
    tree_index += strlen("[D[int]");

    index++;

    parse_multiple_variables(index);

    strcpy(tree + tree_index, "[;]");
        tree_index += 3;

    tree[tree_index++] = ']';
}

void parse_multiple_variables(int index){

    strcpy(tree + tree_index, "[V");
    tree_index += 2;

    parse_variable(index);
    index++;

    if(tokens[index][0] == ';'){
        tree[tree_index++] = ']';
        return;
    }
    
    strcpy(tree + tree_index, "[,]");
    tree_index += 3;
    index++;

    parse_multiple_variables(index);

    tree[tree_index++] = ']';

}

void parse_variable(int index){

    if(!valid_indentifier(tokens[index])){
        fprintf(stderr, "Invalid variable name\n");
        exit(EXIT_FAILURE);
    }

    strcpy(tree + tree_index, "[Var[");
    tree_index += strlen("[Var[");

    strcpy(variables[variable_count].name, tokens[index]);
    variables[variable_count].value = 0;
    variable_count++;

    strcpy(tree + tree_index, tokens[index]);
    tree_index += strlen(tokens[index]);

    tree[tree_index++] = ']';
    tree[tree_index++] = ']';

    return;
}

void parse_statement(int index){

    strcpy(tree + tree_index, "[S");
    tree_index += 2;


    
    tree[tree_index++] = ']';
}

void parse_single_statement(int index){

    strcpy(tree + tree_index, "[Sing");
    tree_index += strlen("[Sing");
    
    int idx = is_keyword(tokens[index]);
    if(idx >= 0 && idx < 3){
        parser_functions[idx](index);
    }

    else{
        
    }

    tree[tree_index++] = ']';
}

void parse_write(int index){

}

void parse_read(int index){

}

void parse_assignment(int index){

}

void parse_for_loop(int index){

}