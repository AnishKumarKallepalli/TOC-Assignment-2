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

#define PROGRAM "P"
#define DECLARATION "D"
#define ASSIGNMENT "Ass"
#define WRITE "W"
#define READ "R"
#define STATEMENT "S"
#define SINGLE_STATEMENT "Sing"
#define VAR_LIST "V"
#define VARIABLE "Var"
#define CONST "Const"
#define LOOP "Loop"

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

typedef struct{
    char name[50];
    node* children[50];
} node;

var variables[MAX_VARIABLES];
int variable_count = 0;


void parse_program(int);
void parse_declaration(int);
void parse_multiple_variables(int);
void parse_variable(int);
void parse_constant(int);
void parse_statement(int);
int parse_single_statement(int);
int parse_expression(int, int*);
int parse_assignment(int);
int parse_read(int);
int parse_write(int);
int parse_for_loop(int);

int (*parser_functions[4])(int index) = {parse_for_loop, parse_read, parse_write, parse_assignment};
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
      if (line[i] == '\t')
        continue;
      
      else if(line[i] == '\n' || line[i] == '\r')
        str[j] = ' ';
      
      else
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

bool is_const(char *str){

	if(str[0] == '0' && str[1] != 0)
		return false;

	for(int i = 0; str[i] != 0; i++){
        if(!isdigit(str[i]))
            return false;
    }

	return true;
}

bool valid_indentifier(char *identifier){

    for(int i = 0; identifier[i] != 0; i++){
        if(!isalpha(identifier[i]))
            return false;
    }

    if(is_keyword(identifier) >= 0)
        return false;

    return true;
}

void add_variable(char *str){

	if(is_variable(str) >= 0){
		fprintf(stderr, "Variable already exists\n");
		exit(EXIT_FAILURE);
	}

	strcpy(variables[variable_count].name, str);
    variables[variable_count].value = 0;
    variable_count++;

	return;
}

int main(int argc, char **argv){

    if (argc < 2)
    {
      perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
      exit(EXIT_FAILURE);
    }
    tokenizer(argv[1]);
    node* root;
    parse_program(0,root);
    printf("%s\n", tree);
    return 0;
}

void parse_program(int index,node* root){
    int c = 0;
    root = malloc(sizeof(node*));
    strcpy(root->name,PROGRAM);
    if(strcmp(tokens[index], "int") == 0){
        parse_declaration(index,root->children[c++]);
    }
    // while (tokens[index++][0] != ';') ;
    if(index == num_tokens){
        return;
    }    
    parse_statement(index,root->children[c++]);
}

void parse_declaration(int index,node* root){

    int c = 0;
    root = malloc(sizeof(node*));
    strcpy(root->name,DECLARATION);
    root->children[c] = malloc(sizeof(node*));
    strcpy(root->children[c++]->name,"int");
    index++;

    parse_multiple_variables(index,root->children[c++]);


    root->children[c] =  malloc(sizeof(node*));
    strcpy(root->children[c]->name,";");
}

void parse_multiple_variables(int index,node* root){

    int c = 0;
    root = malloc(sizeof(node*));
    strcpy(root->name,VAR_LIST);

    parse_variable(index,root->children[c++]);


	  add_variable(tokens[index]);
    index++;
    if(tokens[index][0] == ';'){
        return;
    }
    root->children[c] =  malloc(sizeof(node*));
    strcpy(root->children[c++]->name,",");
    index++;

    parse_multiple_variables(index,root->children[c]);
}

void parse_variable(int index,node* root){
    if(!valid_indentifier(tokens[index])){
        fprintf(stderr, "Invalid variable name\n");
        exit(EXIT_FAILURE);
    }
    int c = 0;
    root = malloc(sizeof(node*));
    strcpy(root->name,VARIABLE);
    strcpy(root->children[c]->name,tokens[index]);
    return;
}

void parse_constant(int index,node* root){

	if(!is_const(tokens[index])){
		fprintf(stderr, "Invalid constant, constant should be a number, given constant is - %s\n", tokens[index]);
		exit(EXIT_FAILURE);
	}
  int c = 0;
  root = malloc(sizeof(node*));
	strcpy(root->name,CONST);
  root->children[c] =  malloc(sizeof(node*));
	strcpy(root->children[c]->name,tokens[index]);
}

void parse_statement(int index,node* root){
  int c = 0;
  root = malloc(sizeof(node*));
  strcpy(root->name,STATEMENT);
	index = parse_single_statement(index,root->children[c++]);
	if(index < num_tokens){
		parse_statement(index,root->children[c++]);
	}
}

int parse_single_statement(int index,node* root){
    int c = 0;
    root = malloc(sizeof(node*));
    strcpy(root->name,SINGLE_STATEMENT);
    int idx = is_keyword(tokens[index]);
    if(idx >= 0 && idx < 3){
        index = parser_functions[idx](index);
    }
    else{
        index = parse_assignment(index,root->children[c++]);
    }
    return index;
}

int parse_write(int index,node* root){
  int c = 0;
  root = malloc(sizeof(node*));
  strcpy(root->name,WRITE);

  root->children[c] =  malloc(sizeof(node*));
  strcpy(root->children[c++]->name,"write");


	if(strcmp(tokens[index], "write")){
		fprintf(stderr, "Should start with write, start with - %s\n", tokens[index]);
		exit(EXIT_FAILURE);
	}
	int idx;
	index++;
	if((idx = is_variable(tokens[index])) >= 0){
		parse_variable(index,root->children[c++]);
		printf("%d\n", variables[idx].value);
	}
	else{
		parse_constant(index,root->children[c++]);
		printf("%s\n", tokens[index]);
	}
  root->children[c] =  malloc(sizeof(node*));
  strcpy(root->children[c++]->name,";");
	return index + 2;
}

int parse_read(int index,node* root){
  int c = 0;
  root = malloc(sizeof(node*));
  strcpy(root->name,READ);
  
  root->children[c] =  malloc(sizeof(node*));
  strcpy(root->children[c++]->name,"read");

	if(strcmp(tokens[index], "read")){
		fprintf(stderr, "Should start with read, start with - %s\n", tokens[index]);
		exit(EXIT_FAILURE);
	}
	index++;
	int idx = is_variable(tokens[index]);

	if(idx < 0){
		fprintf(stderr, "Variable %s does not exist\n", tokens[index]);
		exit(EXIT_FAILURE);
	}

	parse_variable(index,root->children[c++]);

	int num;
	scanf("%d", &num);
	variables[idx].value = num;

	index += 2;
  root->children[c] =  malloc(sizeof(node*));
	strcpy(root->children[c++]->name,";");

	return index;
}

int parse_assignment(int index,node* root){
  
	if(is_variable(tokens[index]) < 0){
		fprintf(stderr, "Variable %s does not exist\n", tokens[index]);
		exit(EXIT_FAILURE);
	}
  int c = 0;
  root = malloc(sizeof(node*));
  strcpy(root->name,ASSIGNMENT);
	parse_variable(index,root->children[c++]);
	index++;

	if(strcmp(tokens[index], "=")){
		fprintf(stderr, "Invalid statement\n");
		exit(EXIT_FAILURE);
	}
  
	index++;

	char *expr[MAX_VARIABLES];

	while(strcmp(tokenspindex, const char *s2)){

	}

	return index;
}

int parse_for_loop(int index){

	return index;
}

int parse_expression(int index, int *value){

}