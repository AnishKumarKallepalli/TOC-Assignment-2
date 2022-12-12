#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
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
#define EXPRESSION "Exp"
#define ADD_SUBTRACT "AS"
#define MUL_DIVIDE "MD"
#define FACT "Fact"
 
/*
P->DS|S
D->int V;
V->Var,V|Var
Var->{a-z}+
Const->{0-9}+
S->SingS|Sing
Sing->Ass|W|R|Loop
W->write Var;|write Const;
R->read Var;
Ass->Var=Exp;
Exp->Exp>As|Exp==As|As
As->As+Md|As-Md|Md
Md->Md/Fac|Md*Fac|Fac
Fac->(Exp)|Var|Const
Loop->for(Ass;Exp;Ass){S};
*/
 
#define LOOP_E1 2
#define LOOP_E2 4
#define LOOP_E3 6
#define LOOP_BODY 9
 
char* tokens[MAX_TOKENS];
int num_tokens = 0;
char* delimiters = " ";
char tree[MAX_TOKENS];
int tree_index = 0;
char keywords[4][10] = { "for", "read", "write", "int" };
 
int for_loop_assignment = 0;
 
typedef struct {
    char name[20];
    int value;
} var;
 
typedef struct node {
    char name[50];
    struct node* children[50];
} node;
 
var variables[MAX_VARIABLES];
int variable_count = 0;
 
 
void parse_program(int, node*);
void parse_declaration(int, node*);
void parse_multiple_variables(int, node*);
void parse_variable(int, node*);
void parse_constant(int, node*);
int parse_statement(int, node*);
int parse_single_statement(int, node*);
int parse_assignment(int, node*);
int parse_read(int, node*);
int parse_write(int, node*);
int parse_for_loop(int, node*);
int parse_full_expression(int, int, node*);
int parse_add_subtract(int, int, node*);
int parse_mul_div(int, int, node*);
int parse_fact(int, int, node*);
bool valid_loop_parameters(int index);
void evaluate_program(node*);
int evaluate_expression(node*);
int evaluate_add_subtract(node*);
int evaluate_multiply_divide(node*);
int evaluate_fact(node*);
int evaluate_variable(node*);
int evaluate_const(node*);
void evaluate_statement(node*);
void evaluate_single_statement(node*);
void evaluate_loop(node*);
void evaluate_assignment(node*);
void evaluate_write(node*);
void evaluate_read(node*);
void evaluate_var_list(node*);
void evaluate_declaration(node*);
 
 
int (*parser_functions[4])(int index, node* root) = { parse_for_loop, parse_read, parse_write, parse_assignment };
 
void add_spaces_around_chars(char* str, const char* chars) {
    // Create a copy of the input string
    char temp[strlen(str) + 1];
    strcpy(temp, str);
 
    // Initialize variables
    char* out = str;
 
    int len = strlen(temp);
 
    // Loop through all characters in the string
    for (int i = 0; i < len; i++)
    {
        if (i < len - 1 && temp[i] == '=' && temp[i + 1] == '=') {
            *out++ = ' ';
            *out++ = '=';
            *out++ = '=';
            *out++ = ' ';
            i++;
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
        } else
        {
            // Copy the character to the output string
            *out++ = temp[i];
        }
    }
 
    // Terminate the output string
    *out = '\0';
}
 
int tokenize(char* input, char* tokens[]) {
    int num_tokens = 0;
 
    char* token = strtok(input, delimiters);
 
    while (token != NULL && num_tokens < MAX_TOKENS)
    {
        tokens[num_tokens++] = token;
        token = strtok(NULL, delimiters);
    }
 
    return num_tokens;
}
 
void tokenizer(char* filename) {
    FILE* in_file = fopen(filename, "r");
    char line[MAX_TOKENS];
    char str[MAX_TOKENS];
    int j = 0;
    while (fgets(line, MAX_LINE_LENGTH, in_file))
    {
        for (int i = 0; i < strlen(line); ++i)
        {
            if (line[i] == '\t')
                continue;
 
            else if (line[i] == '\n' || line[i] == '\r')
                str[j] = ' ';
 
            else
                str[j] = line[i];
            j++;
        }
    }
    add_spaces_around_chars(str, ",;=>+-*/(){}");
    num_tokens = tokenize(str, tokens);
}
 
int is_keyword(char* str) {
 
    for (int i = 0; i < 4; i++) {
        if (!strcmp(str, keywords[i]))
            return i;
    }
 
    return -1;
}
 
int is_variable(char* str) {
 
    for (int i = 0; i < variable_count; i++) {
        if (!strcmp(variables[i].name, str))
            return i;
    }
 
    return -1;
}
 
bool is_const(char* str) {
 
    for (int i = 0; str[i] != 0; i++) {
        if (!isdigit(str[i]))
            return false;
    }
 
    return true;
}
 
bool valid_indentifier(char* identifier) {
 
    for (int i = 0; identifier[i] != 0; i++) {
        if (!isalpha(identifier[i]) || isupper(identifier[i]))
            return false;
    }
 
    if (is_keyword(identifier) >= 0)
        return false;
 
    return true;
}
 
void add_variable(char* str) {
 
    if (is_variable(str) >= 0) {
        fprintf(stderr, "Variable already exists\n");
        exit(EXIT_FAILURE);
    }
 
    strcpy(variables[variable_count].name, str);
    variables[variable_count].value = 0;
    variable_count++;
 
    return;
}
 
void print_syntax_tree(node* root, FILE* out) {
 
    fprintf(out, "[%s", root->name);
 
    int child = 0;
    while (root->children[child] != NULL) {
        print_syntax_tree(root->children[child], out);
        child++;
    }
 
    fprintf(out, "]");
}
 
int main(int argc, char** argv) {
 
    if (argc < 2)
    {
        perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
        exit(EXIT_FAILURE);
    }
    tokenizer(argv[1]);
    // tokenizer("input.txt");
    node* root = malloc(sizeof(node));
 
    FILE* out = fopen("syntaxtree.txt", "w");
 
    parse_program(0, root);
    print_syntax_tree(root, out);
    evaluate_program(root);
    return 0;
}
 
void parse_program(int index, node* root) {
 
    int c = 0;
    strcpy(root->name, PROGRAM);
 
    root->children[c] = malloc(sizeof(node));
 
    /**
        The production is P -> DS | S
        First check if there is a declaration (D), if there is, we parse the declaration statement first
        Then if there is a statement, we will parse it
    */
 
    if (strcmp(tokens[index], "int") == 0) {
        parse_declaration(index, root->children[c++]);
        while (tokens[index++][0] != ';');
    }
 
 
    // Check if other statements are present
    if (index == num_tokens) {
        return;
    }
 
    // Parse the statements
    root->children[c] = malloc(sizeof(node));
    parse_statement(index, root->children[c++]);
}
 
void parse_declaration(int index, node* root) {
 
    int c = 0;
    strcpy(root->name, DECLARATION);
 
    /*
        D -> int V;
    */
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, "int");
    index++;
 
    // Parsing V
    root->children[c] = malloc(sizeof(node));
    parse_multiple_variables(index, root->children[c++]);
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c]->name, ";");
}
 
void parse_multiple_variables(int index, node* root) {
 
    int c = 0;
    strcpy(root->name, VAR_LIST);
 
    /*
        V -> Var, V
        First parse the variable Var and check if there are more variables
        If there are, check if they are seperated by comma
        Parse the remaining variables by recursively calling this function
    */
 
    // Parse a single variable
    root->children[c] = malloc(sizeof(node));
    parse_variable(index, root->children[c++]);
 
    index++;
 
    // Check if more variables are present
    if (tokens[index][0] == ';') {
        return;
    }
 
    // Check if the variables are seperated by comma
    if (tokens[index][0] != ',') {
        fprintf(stderr, "Variables should be seperated by ','\n");
        exit(EXIT_FAILURE);
    }
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, ",");
    index++;
 
    // Recursively call the function
    root->children[c] = malloc(sizeof(node));
    parse_multiple_variables(index, root->children[c]);
}
 
void parse_variable(int index, node* root) {
 
    /*
        Var -> {a-z}+
        Check if it is a valid identifier, if it is add to the parse tree
    */
 
    if (!valid_indentifier(tokens[index])) {
        fprintf(stderr, "Invalid variable name\n");
        exit(EXIT_FAILURE);
    }
 
    int c = 0;
    strcpy(root->name, VARIABLE);
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, tokens[index]);
 
    return;
}
 
void parse_constant(int index, node* root) {
 
    if (!is_const(tokens[index])) {
        fprintf(stderr, "Invalid constant, constant should be a number, given constant is - %s\n", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    int c = 0;
    strcpy(root->name, CONST);
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c]->name, tokens[index]);
}
 
int parse_statement(int index, node* root) {
 
    int c = 0;
    strcpy(root->name, STATEMENT);
 
    /*
        S -> Sing S | Sing
        Since S will always have a single statement, we will parse Sing first
        Then check if more statements are present and if there are parse them by recursively calling this function
    */
 
    // Parse a single statement
    root->children[c] = malloc(sizeof(node));
    index = parse_single_statement(index, root->children[c++]);
 
    // Check if there are more statements
    if (index < num_tokens && tokens[index][0] != '}') {
        root->children[c] = malloc(sizeof(node));
        // Recursively call the function
        index = parse_statement(index, root->children[c]);
    }
 
    return index;
}
 
int parse_single_statement(int index, node* root) {
 
    int c = 0;
    strcpy(root->name, SINGLE_STATEMENT);
 
    /*
        Sing -> R | W | Loop | Ass
 
        Check if statement starts with a keyword, if it does then call the corresponding parser function
        If it is not a keyword check if it is a keyword and if it is call the parser for assignment statement
 
    */
 
    int idx = is_keyword(tokens[index]);
 
    // Check if it starts with keyword
    if (idx >= 0 && idx < 3) {
        root->children[c] = malloc(sizeof(node));
        // Call the corresponding parser
        index = parser_functions[idx](index, root->children[c++]);
    }
 
    // Check if it starts with a identifier
    else if (valid_indentifier(tokens[index])) {
        root->children[c] = malloc(sizeof(node));
        // Call the parser for assignment
        index = parse_assignment(index, root->children[c++]);
    } else {
 
        // Error is detectedelse{
        fprintf(stderr, "Unrecognized symbol '%s'", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    return index;
}
 
int parse_write(int index, node* root) {
 
    if (strcmp(tokens[index], "write")) {
        fprintf(stderr, "Should start with write, start with - %s\n", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    /*
        W -> write Var; | write Const;
        The children of W in the parse tree will be -
        1) write
        2) Var | Const
        3) ;
 
        We first check if we are writing a variable or a constant. Once determined, the corresponging parser functions will be called
        Else an error is shown
    */
 
 
    int c = 0;
    strcpy(root->name, WRITE);
 
    // Add write to the parse tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, "write");
 
    int idx;
    index++;
 
    // Check if we are writing a variable
    if (valid_indentifier(tokens[index])) {
        root->children[c] = malloc(sizeof(node));
        parse_variable(index, root->children[c++]);
    }
 
    // Check if a constant is begin written
    else if (is_const(tokens[index])) {
        root->children[c] = malloc(sizeof(node));
        parse_constant(index, root->children[c++]);
    }
 
    else {
        fprintf(stderr, "write should be followed either by a variable or a integer constant, %s is neither\n", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    if (tokens[index + 1][0] != ';') {
        fprintf(stderr, "Write statement should terminate with ';'\n");
        exit(EXIT_FAILURE);
    }
 
    // Add ; to the parse tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, ";");
 
    return index + 2;
}
 
int parse_read(int index, node* root) {
 
    if (strcmp(tokens[index], "read")) {
        fprintf(stderr, "Should start with read, start with - %s\n", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    /*
        W -> read Var;
        The children of W in the parse tree will be -
        1) read
        2) Var
        3) ;
    */
 
    int c = 0;
    strcpy(root->name, READ);
 
    // Add read to the parse tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, "read");
 
    index++;
 
    if (!valid_indentifier(tokens[index])) {
        fprintf(stderr, "%s is not a valid variable. Variables should be of the form {a-z}+\n", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    // Parse the variable
    root->children[c] = malloc(sizeof(node));
    parse_variable(index, root->children[c++]);
 
    if (tokens[index + 1][0] != ';') {
        fprintf(stderr, "Read statement should terminate with ';'\n");
        exit(EXIT_FAILURE);
    }
 
    index += 2;
 
    // Add ; to the parse tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, ";");
 
    return index;
}
 
int parse_assignment(int index, node* root) {
 
    if (!valid_indentifier(tokens[index])) {
        fprintf(stderr, "%s is not a valid variable. Variables should be of the form {a-z}+\n", tokens[index]);
        exit(EXIT_FAILURE);
    }
 
    /*
        Ass -> Var = Exp;
        First parse the variable present on the LHS
        Then check if it is an assignment expression by checking for the '=' symbol and add it to the parse tree
 
        Now there will two cases both of which will be handled seperately
 
            1) Last parameter of a for loop: Here the statement will not end with a semi-colon, it will end with a ')'
                - Find the index of the ')' that comes at the end of the statement by checking for other parentheses that might be present
                - If not present, or if the sequence is not balanced print error
 
            2) Normal assignment in which case it will end with a semi-colon
                - Find the index of the semi-colon at the end of the statement
                - If not present, print error
 
        Finally parse the expression. Pass the end of the expression also as a parameter.
    */
 
    int c = 0;
    strcpy(root->name, ASSIGNMENT);
 
    // Parse the variable in the LHS
    root->children[c] = malloc(sizeof(node));
    parse_variable(index, root->children[c++]);
    index++;
 
    // Check if variable is followed by '='
    if (strcmp(tokens[index], "=")) {
        fprintf(stderr, "Invalid assignment statement, Assignment statement must contain '='\n");
        exit(EXIT_FAILURE);
    }
 
    // Add '=' to the parse tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, "=");
 
    index++;
 
    // Position of the last symbol of the expression
    int lim = index;
 
    // Check if it is the last parameter of a for loop
    if (for_loop_assignment == 2) {
 
        int bracket_cnt = 0;
 
        // Check for ')' while also checking if the parantheses are balanced
        while (lim < num_tokens && (tokens[lim][0] != ')' || bracket_cnt) && bracket_cnt >= 0) {
 
            tokens[lim][0] == '(' && bracket_cnt++;
 
            tokens[lim][0] == ')' && bracket_cnt--;
 
            lim++;
        }
 
        if (bracket_cnt) {
            fprintf(stderr, "Incorrect bracket sequence - 4\n");
            exit(EXIT_FAILURE);
        }
    }
 
    else {
        // Check for the semi-colon to get the end of the expression
        while (lim < num_tokens && tokens[lim][0] != ';') lim++;
        if (lim == num_tokens || tokens[lim][0] != ';') {
            fprintf(stderr, "Assignment statement should terminate with ';'\n");
            exit(EXIT_FAILURE);
        }
    }
 
    // Parse the expression
    root->children[c] = malloc(sizeof(node));
    index = parse_full_expression(index, lim, root->children[c++]);
 
    if (!for_loop_assignment) {
        root->children[c] = malloc(sizeof(node));
        strcpy(root->children[c]->name, ";");
    }
 
    for_loop_assignment = 0;
 
    assert(index == lim + 1);
 
    return index;
}
 
int parse_full_expression(int index, int lim, node* root) {
 
    int c = 0;
    strcpy(root->name, EXPRESSION);
 
    /*
        Exp -> Exp > AS | Exp == As | As
 
        Find the operator which here are '>' and '=='. Since it is right recursive, we just need to find the first occurence of any of the two operators
        We find them while also checking if the parentheses are balanced
 
        If not found, we parse this expression by parsing As
 
        If they are found, we divide the expression into two, the left operand and the right operand
        To parse the left operand, we recursively call the function
        To parse the right operand, we call the parser for As
 
    */
 
    int operator_pos = -1;
 
    int cnt = 0;
 
 
    // Find the position of the operators while checking for balanced parentheses.
    for (int i = index; i < lim; i++) {
        cnt += strcmp(tokens[i], "(") == 0;
        cnt -= strcmp(tokens[i], ")") == 0;
        if (!cnt && (!strcmp(tokens[i], ">") || !strcmp(tokens[i], "=="))) {
            operator_pos = i;
        }
    }
 
 
    if (cnt) {
        fprintf(stderr, "Incorrect bracket sequence - 1\n");
        exit(EXIT_FAILURE);
    }
 
    // If operator is not found, parse the expression using the parser for As
    if (operator_pos == -1) {
        root->children[c] = malloc(sizeof(node));
        index = parse_add_subtract(index, lim, root->children[c++]);
        return index;
    }
 
    // If the operator is found, we parse the left operand by recursively calling this function
    root->children[c] = malloc(sizeof(node));
    index = parse_full_expression(index, operator_pos, root->children[c++]);
 
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, tokens[operator_pos]);
 
    // If the operator is found, parse the right operand by calling the parser for As
    root->children[c] = malloc(sizeof(node));
    index = parse_add_subtract(operator_pos + 1, lim, root->children[c++]);
 
    return index;
}
 
int parse_add_subtract(int index, int lim, node* root) {
 
    int c = 0;
    strcpy(root->name, ADD_SUBTRACT);
 
    int operator_pos = -1;
 
    int cnt = 0;
 
    for (int i = index; i < lim; i++) {
        cnt += strcmp(tokens[i], "(") == 0;
        cnt -= strcmp(tokens[i], ")") == 0;
        if (!cnt && (!strcmp(tokens[i], "+") || !strcmp(tokens[i], "-"))) {
            operator_pos = i;
        }
    }
 
 
    if (cnt) {
        fprintf(stderr, "Incorrect bracket sequence - 1\n");
        exit(EXIT_FAILURE);
    }
 
    if (operator_pos == -1) {
        root->children[c] = malloc(sizeof(node));
        index = parse_mul_div(index, lim, root->children[c++]);
        return index;
    }
 
    root->children[c] = malloc(sizeof(node));
    index = parse_add_subtract(index, operator_pos, root->children[c++]);
 
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, tokens[operator_pos]);
 
    root->children[c] = malloc(sizeof(node));
    index = parse_mul_div(operator_pos + 1, lim, root->children[c++]);
 
    return index;
}
 
int parse_mul_div(int index, int lim, node* root) {
 
    int c = 0;
    strcpy(root->name, MUL_DIVIDE);
 
    int operator_pos = -1;
 
    int cnt = 0;
 
    for (int i = index; i < lim; i++) {
        cnt += strcmp(tokens[i], "(") == 0;
        cnt -= strcmp(tokens[i], ")") == 0;
        if (!cnt && (!strcmp(tokens[i], "*") || !strcmp(tokens[i], "/"))) {
            operator_pos = i;
        }
    }
 
 
    if (cnt) {
        fprintf(stderr, "Incorrect bracket sequence - 1\n");
        exit(EXIT_FAILURE);
    }
 
    if (operator_pos == -1) {
        root->children[c] = malloc(sizeof(node));
        index = parse_fact(index, lim, root->children[c++]);
        return index;
    }
 
    root->children[c] = malloc(sizeof(node));
    index = parse_mul_div(index, operator_pos, root->children[c++]);
 
 
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, tokens[operator_pos]);
 
    root->children[c] = malloc(sizeof(node));
    index = parse_fact(operator_pos + 1, lim, root->children[c++]);
 
    return index;
}
 
int parse_fact(int index, int lim, node* root) {
 
    int c = 0;
    strcpy(root->name, FACT);
 
    /*
        Fact -> (Expr) | Var | Const
 
        Three cases to handle each of the above cases
    */
 
    // Check if of the form Expr -> (Expr)
    if (!strcmp(tokens[index], "(")) {
 
        if (strcmp(tokens[lim - 1], ")")) {
            fprintf(stderr, "Invalid Expression - 1\n");
            exit(EXIT_FAILURE);
        }
 
        root->children[c] = malloc(sizeof(node));
        strcpy(root->children[c++]->name, "(");
 
        root->children[c] = malloc(sizeof(node));
        parse_full_expression(index + 1, lim - 1, root->children[c++]);
 
        root->children[c] = malloc(sizeof(node));
        strcpy(root->children[c++]->name, ")");
    }
 
    else if (index != lim - 1) {
        fprintf(stderr, "Invalid expression - 2\n");
        exit(EXIT_FAILURE);
    }
 
    // Check if of the form Expr -> Var
    else if (valid_indentifier(tokens[index])) {
        root->children[c] = malloc(sizeof(node));
        parse_variable(index, root->children[c++]);
    }
 
    // Check if of the form Expr -> Const
    else if (is_const(tokens[index])) {
        root->children[c] = malloc(sizeof(node));
        parse_constant(index, root->children[c++]);
    }
 
    else {
        fprintf(stderr, "Invalid expression - 3\n");
        exit(EXIT_FAILURE);
    }
 
    return lim + 1;
}
 
int parse_for_loop(int index, node* root) {
 
    if (strcmp(tokens[index], "for")) {
        fprintf(stderr, "For loop should begin with keword 'for'");
        exit(EXIT_FAILURE);
    }
 
    /*
        Loop -> for ( Ass ; Exp ; Ass ) { S };
        Children for Loop in the parse tree are -
        1) for
        2) (
        3) Ass
        4) ;
        5) Exp
        6) ;
        7) Ass
        8) )
        9) {
        10 S
        11) }
        12) ;
 
        Check if the loop contains three statements as its parameters.
        Check each token individually and them direcly or by parsing them further
        Ass, Exp, Ass, S will require furthure parsing while others are directly added.
 
    */
 
    int c = 0;
    strcpy(root->name, LOOP);
 
    // 1) Add for to the syntax tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, "for");
 
    // 2) Add ( to the syntax tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, "(");
 
    index += 2;
 
    // Check if the loop has the right number of statements
    if (!valid_loop_parameters(index)) {
        fprintf(stderr, "for loop should be of the form 'for(A|E; E; A|E){S}'\n");
        exit(EXIT_FAILURE);
    }
 
    // 3) Parse the assignment statement
    assert(LOOP_E1 == c);
    for_loop_assignment = 1;
    root->children[c] = malloc(sizeof(node));
    index = parse_assignment(index, root->children[c++]);
    for_loop_assignment = 0;
 
    // 4) Add ; to the syntax tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, ";");
 
    // 5) Parse the expression by finding the position of the semi-colon at the end of the expression
    assert(LOOP_E2 == c);
    int lim = index;
    root->children[c] = malloc(sizeof(node));
    while (lim < num_tokens && tokens[lim][0] != ';') lim++; // Find the position of the semi-colon
    index = parse_full_expression(index, lim, root->children[c++]);
 
    // 6) Add ; to the syntax tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, ";");
 
    // 7) Parse the assignment statement. Set for_loop_assignment to denote that this is the last parameter in the for loop
    assert(LOOP_E3 == c);
    for_loop_assignment = 2;
    root->children[c] = malloc(sizeof(node));
    index = parse_assignment(index, root->children[c++]);
    for_loop_assignment = 0; // Reset 
 
    // 8) Add ) to the syntax tree
    root->children[c] = malloc(sizeof(node));
    strcpy(root->children[c++]->name, ")");
 
    // 9) Add { to the syntax tree
    root->children[c] = malloc(sizeof(node));
    assert(strcmp(tokens[index], "{") == 0 && "For loop body should be enclosed in '{}' - Missing '{'");
    strcpy(root->children[c++]->name, "{");
    index++;
 
    // 10) Parse the loop body statements.
    root->children[c] = malloc(sizeof(node));
    index = parse_statement(index, root->children[c++]);
 
    // 11) Add } to the syntax tree
    root->children[c] = malloc(sizeof(node));
    assert(strcmp(tokens[index], "}") == 0 && "For loop body should be enclosed in '{}' - Missing '}'");
    strcpy(root->children[c++]->name, "}");
    index++;
 
    // 12) Add ; to the syntax tree
    root->children[c] = malloc(sizeof(node));
    assert(strcmp(tokens[index], ";") == 0 && "Loop statement should terminate with ';'");
    strcpy(root->children[c++]->name, ";");
    index++;
 
    return index;
}
 
bool valid_loop_parameters(int index) {
 
    // Check if for loop has 3 argument, by finding the number of semi-colon 
 
    int statement_cnt = 1;
    int bracket_cnt = 1;
 
    while (index < num_tokens && bracket_cnt) {
        if (tokens[index][0] == ';')
            statement_cnt++;
 
        else if (tokens[index][0] == '(')
            bracket_cnt++;
 
        else if (tokens[index][0] == ')')
            bracket_cnt--;
 
        index++;
    }
 
    return index < num_tokens&& bracket_cnt == 0 && statement_cnt == 3;
}
 
int evaluate_variable(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In Var %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 1);
 
    int idx = is_variable(root->children[0]->name);
    if (idx == -1) {
        printf("Variable \"%s\" wasn't declared\n", root->children[0]->name);
        exit(EXIT_FAILURE);
    }
    return variables[idx].value;
}
 
int evaluate_const(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In Const %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 1);
 
    int res = 0;
    for (int i = 0; root->children[0]->name[i] != '\0'; ++i)
        res = (res * 10) + (root->children[0]->name[i] - '0');
 
    return res;
}
 
int evaluate_fact(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In Fact %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3 || len == 1);
 
    if (len == 1) {
        if (strcmp(root->children[0]->name, VARIABLE) == 0)
            return evaluate_variable(root->children[0]);
        else if (strcmp(root->children[0]->name, CONST) == 0)
            return evaluate_const(root->children[0]);
        else
            assert(false && "Parse tree wrong in fact expression");
    } else {
        if (strcmp(root->children[0]->name, "(") == 0 && strcmp(root->children[2]->name, ")") == 0) {
            return evaluate_expression(root->children[1]);
        } else
            assert(false && "fact have only (EXP) expression");
    }
 
    // unreachable as assert
    return -1;
}
 
int evaluate_multiply_divide(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In Multiply Divide %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3 || len == 1);
 
    if (len == 1) {
        if (strcmp(root->children[0]->name, FACT) == 0)
            return evaluate_fact(root->children[0]);
        else
            assert(false && "Parse tree wrong in mul div expression");
    } else {
        if (strcmp(root->children[1]->name, "*") == 0) {
            return evaluate_multiply_divide(root->children[0]) * evaluate_fact(root->children[2]);
        } else if (strcmp(root->children[1]->name, "/") == 0) {
            return evaluate_multiply_divide(root->children[0]) / evaluate_fact(root->children[2]);
        } else
            assert(false && "mul div have only * and / expression");
    }
 
    // unreachable as assert
    return -1;
}
 
int evaluate_add_subtract(node* root) {
    // root = root->children[0];
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In add subtract %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3 || len == 1);
 
    if (len == 1) {
        if (strcmp(root->children[0]->name, MUL_DIVIDE) == 0)
            return evaluate_multiply_divide(root->children[0]);
        else
            assert(false && "Parse tree wrong in add sub expression");
    } else {
        if (strcmp(root->children[1]->name, "+") == 0) {
            return evaluate_add_subtract(root->children[0]) + evaluate_multiply_divide(root->children[2]);
        } else if (strcmp(root->children[1]->name, "-") == 0) {
            return evaluate_add_subtract(root->children[0]) - evaluate_multiply_divide(root->children[2]);
        } else
            assert(false && "add sub have only + and - expression");
    }
 
    // unreachable as assert
    return -1;
}
 
int evaluate_expression(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In eval expression %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3 || len == 1);
 
    if (len == 1) {
        if (strcmp(root->children[0]->name, ADD_SUBTRACT) == 0)
            return evaluate_add_subtract(root->children[0]);
        else
            assert(false && "Parse tree wrong in eval expression");
    } else {
        if (strcmp(root->children[1]->name, "==") == 0) {
            return evaluate_expression(root->children[0]) == evaluate_add_subtract(root->children[2]);
        } else if (strcmp(root->children[1]->name, ">") == 0) {
            // printf("Result of add Subtract in eval %d %d %d\n", evaluate_expression(root->children[0]), evaluate_add_subtract(root->children[2]), evaluate_expression(root->children[0]) > evaluate_add_subtract(root->children[2]));
            return evaluate_expression(root->children[0]) > evaluate_add_subtract(root->children[2]);
        } else
            assert(false && "eval have only == and > expression");
    }
 
    // unreachable as assert
    return -1;
}
 
void evaluate_read(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In read %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3);
 
    int idx = is_variable(root->children[1]->children[0]->name);
    assert(idx != -1 && "Variable doesn't exist");
    scanf("%d", &variables[idx].value);
 
    if (variables[idx].value < 0) {
        fprintf(stderr, "Variable '%s' read negative value\n", root->children[0]->children[0]->name);
        exit(EXIT_FAILURE);
    }
}
 
void evaluate_write(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In write %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3);
 
    if (strcmp(root->children[1]->name, CONST) == 0)
        printf("%s\n", root->children[1]->children[0]->name);
    else if (strcmp(root->children[1]->name, VARIABLE) == 0) {
        int idx = is_variable(root->children[1]->children[0]->name);
        assert(idx != -1 && "Variable doesn't exist");
        printf("%d\n", variables[idx].value);
    } else
        assert(false && "Parse tree wrong in write");
}
 
void evaluate_assignment(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In assignment %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3 || len == 4);
 
    int idx = is_variable(root->children[0]->children[0]->name);
    // printf("Assignment Variable Name %s %d\n", root->children[0]->children[0]->name, idx);
    assert(idx != -1 && "Variable doesn't exist");
    variables[idx].value = evaluate_expression(root->children[2]);
 
    if (variables[idx].value < 0) {
        fprintf(stderr, "Variable '%s' assigned negative value\n", root->children[0]->children[0]->name);
        exit(EXIT_FAILURE);
    }
}
 
void evaluate_loop(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In loop %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 12);
 
    for (evaluate_assignment(root->children[2]); evaluate_expression(root->children[4]); evaluate_assignment(root->children[6]))
        evaluate_statement(root->children[9]);
}
 
void evaluate_single_statement(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In statement %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 1);
 
 
    if (strcmp(root->children[0]->name, ASSIGNMENT) == 0)
        evaluate_assignment(root->children[0]);
    else if (strcmp(root->children[0]->name, READ) == 0)
        evaluate_read(root->children[0]);
    else if (strcmp(root->children[0]->name, WRITE) == 0)
        evaluate_write(root->children[0]);
    else if (strcmp(root->children[0]->name, LOOP) == 0)
        evaluate_loop(root->children[0]);
    else
        assert(false && "Single Statement wrong");
}
 
void evaluate_statement(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In single statement %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 1 || len == 2);
 
    for (int child = 0; root->children[child]; ++child) {
        if (strcmp(root->children[child]->name, STATEMENT) == 0)
            evaluate_statement(root->children[child]);
        else if (strcmp(root->children[child]->name, SINGLE_STATEMENT) == 0)
            evaluate_single_statement(root->children[child]);
        else
            assert(false && "Parse tree wrong in statement");
    }
}
 
void evaluate_var_list(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In Var list %s\n", root->children[child]->name);
        len++;
    }
 
    assert(len % 2);
 
    for (int child = 0; child < len; child += 2) {
        if (strcmp(root->children[child]->name, VAR_LIST) == 0)
            evaluate_var_list(root->children[child]);
        else if (strcmp(root->children[child]->name, VARIABLE) == 0)
            add_variable(root->children[child]->children[0]->name);
        else
            assert(false && "Parse tree wrong in Var list");
    }
}
 
void evaluate_declaration(node* root) {
    int len = 0;
    for (int child = 0; root->children[child]; ++child) {
        // printf("In declaration %s\n", root->children[child]->name);
        len++;
    }
    assert(len == 3);
    if (strcmp(root->children[1]->name, VAR_LIST) == 0)
        evaluate_var_list(root->children[1]);
    else
        assert(false && "Parse tree wrong in declaration list");
}
 
void evaluate_program(node* root) {
    if (!root) return;
 
    if (strcmp(root->name, PROGRAM) == 0) {
        int len = 0;
        for (int child = 0; root->children[child]; ++child) len++;
        assert(len == 1 || len == 2);
 
        evaluate_program(root->children[0]);
        if (len == 2)
            evaluate_program(root->children[1]);
    } else if (strcmp(root->name, DECLARATION) == 0) {
        evaluate_declaration(root);
    } else if (strcmp(root->name, STATEMENT) == 0) {
        evaluate_statement(root);
    }
}
