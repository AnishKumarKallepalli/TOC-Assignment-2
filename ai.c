
#include <ctype.h> // Add the ctype.h header file for the isalpha() and isdigit() functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the structure of a syntax tree node */
typedef struct tree_node {
  char *value;
  struct tree_node *left;
  struct tree_node *right;
} tree_node;

/* Declare the variables array */
int variables[26];

/* Function prototypes */
tree_node *parse_statements(char *P);
tree_node *parse_read_statement(char *P);
tree_node *parse_write_statement(char *P);
tree_node *parse_assignment_statement(char *P);
tree_node *parse_for_loop(char *P);
tree_node *parse_expression(char *P);
tree_node *parse_term(char *P);
tree_node *parse_factor(char *P);
tree_node *parse_declaration_statement(char *P);
tree_node *parse_variable_list(char *P) ;
void eval(tree_node *node);
void eval_program(tree_node *node);
void eval_declaration_statement(tree_node *node);
void eval_variable_list(tree_node *node);
void eval_statements(tree_node *node);
void eval_read_statement(tree_node *node);
void eval_write_statement(tree_node *node);
void eval_assignment_statement(tree_node *node);
void eval_for_loop(tree_node *node);
int eval_expression(tree_node *node);
int eval_term(tree_node *node);
int eval_factor(tree_node *node);

/* Parses a program in basic C and returns a syntax tree */
tree_node *parse_program(char *P) {
  char *p = P;
  tree_node *node = NULL;

  /* Check if the program starts with a declaration statement */
  if (strncmp(p, "int", 3) == 0) {
    /* If the program starts with a declaration statement, parse it and set it as the root of the syntax tree */
    node = parse_declaration_statement(p);

    /* Parse the rest of the program and set it as the right child of the root node */
    p = strchr(p, ';') + 1;
    node->right = parse_program(p);
  }
  else {
    /* If the program does not start with a declaration statement, parse it as a sequence of statements and set it as the root of the syntax tree */
    node = parse_statements(p);
  }

  return node;
}

/* Parses a declaration statement in basic C and returns a syntax tree */
tree_node *parse_declaration_statement(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to "int" */
  node->value = "int";

  /* Parse the variable list and set it as the left child of the node */
  p += 3;
  node->left = parse_variable_list(p);

  return node;
}

/* Parses a variable list in basic C and returns a syntax tree */
tree_node *parse_variable_list(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to the first variable in the list */
  while (*p == ' ') p++;
  node->value = p;
  while (isalpha(*p)) p++;
  *p = '\0';

  /* Parse the rest of the list and set it as the right child of the node */
  p++;
  if (*p == ',') {
    p++;
    node->right = parse_variable_list(p);
  }
  else {
    node->right = NULL;
  }

  return node;
}

/* Parses a statement in basic C and returns a syntax tree */
tree_node *parse_statement(char *P) {
  char *p = P;
  tree_node *node = NULL;

  /* Check if the statement is a declaration statement */
  if (strncmp(p, "int ", 4) == 0) {
    node = parse_declaration_statement(p);
  }
  /* Check if the statement is a read statement */
  else if (strncmp(p, "read ", 5) == 0) {
    node = parse_read_statement(p);
  }
  /* Check if the statement is a write statement */
  else if (strncmp(p, "write ", 6) == 0) {
    node = parse_write_statement(p);
  }
  /* Check if the statement is an assignment statement */
  else if (isalpha(p[0]) && p[1] == '=') {
    node = parse_assignment_statement(p);
  }
  /* Otherwise, the statement must be a for-loop */
  else {
    node = parse_for_loop(p);
  }

  return node;
}


/* Parses a variable in basic C and returns a syntax tree */
tree_node *parse_variable(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to the variable name */
  node->value = p;

  /* Set the left and right children of the node to NULL */
  node->left = NULL;
  node->right = NULL;

  return node;
}

/* Parses a read statement in basic C and returns a syntax tree */
tree_node *parse_read_statement(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to "read" */
  node->value = "read";

  /* Parse the variable and set it as the left child of the node */
  p += 5;
  node->left = parse_variable(p);

  /* Return the node */
  return node;
}

/* Parses a write statement in basic C and returns a syntax tree */
tree_node *parse_write_statement(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to "write" */
  node->value = "write";

  /* Check if the argument of the write statement is a variable or a constant */
  p += 6;
  if (isalpha(*p)) {
    /* If the argument is a variable, parse it and set it as the left child of the node */
    node->left = parse_variable(p);
  }
  else {
    /* If the argument is a constant, create a new node for it and set it as the left child of the write node */
    tree_node *const_node = (tree_node *) malloc(sizeof(tree_node));
    const_node->value = p;
    const_node->left = NULL;
    const_node->right = NULL;
    node->left = const_node;
  }

  /* Return the node */
  return node;
}

/* Parses an assignment statement in basic C and returns a syntax tree */
tree_node *parse_assignment_statement(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to "=" */
  node->value = "=";

  /* Parse the variable and set it as the left child of the node */
  node->left = parse_variable(p);

  /* Parse the expression and set it as the right child of the node */
  p += 2;
  node->right = parse_expression(p);

  return node;
}

/* Parses an expression in basic C and returns a syntax tree */
tree_node *parse_expression(char *P) {
  char *p = P;
  tree_node *node = NULL;

  /* Check if
the expression contains a binary operator */
  char *op = NULL;
  if ((op = strchr(p, '+')) || (op = strchr(p, '-')) || (op = strchr(p, '>')) || (op = strchr(p, '='))) {
    /* If the expression contains a binary operator, parse the left and right operands of the operator */
    node = (tree_node *) malloc(sizeof(tree_node));
    node->value = op;
    node->left = parse_expression(p);
    node->right = parse_expression(op + 1);
  }
  else {
    /* If the expression does not contain a binary operator, parse it as a term */
    node = parse_term(p);
  }

  return node;
}

/* Parses a term in basic C and returns a syntax tree */
tree_node *parse_term(char *P) {
  char *p = P;
  tree_node *node = NULL;

  /* Check if the term contains a binary operator */
  char *op = NULL;
  if ((op = strchr(p, '*')) || (op = strchr(p, '/'))) {
    /* If the term contains a binary operator, parse the left and right operands of the operator */
    node = (tree_node *) malloc(sizeof(tree_node));
    node->value = op;
    node->left = parse_term(p);
    node->right = parse_term(op + 1);
  }
  else {
    /* If the term does not contain a binary operator, parse it as a factor */
    node = parse_factor(p);
  }

  return node;
}

/* Parses a factor in basic C and returns a syntax tree */
tree_node *parse_factor(char *P) {
  char *p = P;
  tree_node *node = NULL;

  /* Check if the factor is a variable or a constant */
  if (isalpha(*p)) {
    /* If the factor is a variable, parse it and return it as the node */
    node = parse_variable(p);
  }
  else if (isdigit(*p)) {
    /* If the factor is a constant, create a new node for it and return it */
    node = (tree_node *) malloc(sizeof(tree_node));
    node->value = p;
    node->left = NULL;
    node->right = NULL;
  }
  else {
    /* If the factor is an expression in parentheses, parse the expression and return it as the node */
    p++;
    node = parse_expression(p);
    p = strchr(p, ')');
  }

  return node;
}

/* Parses a for-
loop in basic C and returns a syntax tree */
tree_node *parse_for_loop(char *P) {
  char *p = P;
  tree_node *node = (tree_node *) malloc(sizeof(tree_node));

  /* Set the value of the node to "for" */
  node->value = "for";

  /* Parse the initialization, condition, and update expressions and set them as the left, middle, and right children of the node, respectively */
  p += 4;
  node->left = parse_expression(p);
  p = strchr(p, ';') + 1;
  tree_node *cond_node = (tree_node *) malloc(sizeof(tree_node));
  cond_node->value = ";";
  cond_node->left = parse_expression(p);
  cond_node->right = NULL;
  node->right = cond_node;
  p = strchr(p, ';') + 1;
  tree_node *update_node = (tree_node *) malloc(sizeof(tree_node));
  update_node->value = ";";
  update_node->left = parse_expression(p);
  update_node->right = NULL;
  cond_node->right = update_node;

  /* Parse the statements inside the for-loop and set them as the right child of the update expression node */
  p = strchr(p, '{') + 1;
  update_node->right = parse_statements(p);

  return node;
}

/* Parses a sequence of statements in basic C and returns a syntax tree */
tree_node *parse_statements(char *P) {
char *p = P;
  tree_node *node = NULL;

  /* Check if there is more than one statement */
  char *semicolon = strchr(p, ';');
  if (semicolon) {
    /* If there are more than one statement, parse the first statement and set it as the left child of the node */
    node = (tree_node *) malloc(sizeof(tree_node));
    node->value = ";";
    node->left = parse_statement(p);

    /* Parse the rest of the statements and set them as the right child of the node */
    p = semicolon + 1;
    node->right = parse_statements(p);
  }
  else {
    /* If there is only one statement, parse it and return it as the node */
    node = parse_statement(p);
  }

  return node;
}

/* Traverses the syntax tree and prints its nodes in preorder */
void print_syntax_tree(tree_node *root) {
  /* Print the value of the current node */
  printf("%s ", root->value);

  /* Traverse the left subtree */
  if (root->left) {
    print_syntax_tree(root->left);
  }

  /* Traverse the right subtree */
  if (root->right) {
    print_syntax_tree(root->right);
  }
}

/* Evaluates the syntax tree and simulates the execution of the program */
void eval_syntax_tree(tree_node *root) {
  /* Check the value of the current node */
  if (strcmp(root->value, "int") == 0) {
    /* If the node is a declaration statement, initialize the variables in the variable list */
    eval_declaration_statement(root->left);
  }
  else if (strcmp(root->value, "read") == 0) {
    /* If the node is a read statement, read a value from the user and assign it to the variable */
    eval_read_statement(root->left);
  }
  else if (strcmp(root->value, "write") == 0) {
    /* If the node is a write statement, print the value of the variable or constant to the screen */
    eval_write_statement(root->left);
  }
  else if (strcmp(root->value, "=") == 0) {
    /* If the node is an assignment statement, evaluate the expression and assign the result to the variable */
    eval_assignment_statement(root);
  }
  else if (strcmp(root->value, "for") == 0) {
    /* If the node is a for-loop, evaluate the loop */
    eval_for_loop(root);
  }
  else {
    /* Otherwise, the node must be an expression, so evaluate it */
    eval_expression(root);
  }
}

/* Initializes the variables in a variable list */
void eval_declaration_statement(tree_node *var_list) {
/* Initialize the variable in the variable list to 0 */
  while (var_list) {
    variables[var_list->value[0] - 'a'] = 0;
    var_list = var_list->right;
  }
}

/* Reads a value from the user and assigns it to a variable */
void eval_read_statement(tree_node *var) {
  /* Read the value from the user and assign it to the variable */
  int value;
  scanf("%d", &value);
  variables[var->value[0] - 'a'] = value;
}

/* Prints the value of a variable or constant to the screen */
void eval_write_statement(tree_node *var_or_const) {
  /* Check if the argument is a variable or a constant */
  if (var_or_const->left) {
    /* If the argument is a variable, print its value */
    printf("%d", variables[var_or_const->value[0] - 'a']);
  }
  else {
    /* If the argument is a constant, print it */
    printf("%s", var_or_const->value);
  }
}

/* Evaluates an assignment statement */
void eval_assignment_statement(tree_node *assignment) {
  /* Evaluate the expression and assign the result to the variable */
  int value = eval_expression(assignment->right);
  variables[assignment->left->value[0] - 'a'] = value;
}

/* Evaluates a for-loop */
void eval_for_loop(tree_node *for_loop) {
  /* Evaluate the initialization expression */
  eval_expression(for_loop->left);

  /* Keep looping until the condition expression evaluates to 0 */
  int cond = eval_expression(for_loop->right->left);
  while (cond) {
    /* Evaluate the statements inside the loop */
    eval_syntax_tree(for_loop->right->right->right);

    /* Evaluate the update expression */
    eval_expression(for_loop->right->right->left);

    /* Re-evaluate the condition expression */
    cond = eval_expression(for_loop->right->left);
  }
}

/* Evaluates an expression */
int eval_expression(tree_node *expr) {
  int value = 0;

  /* Check if the expression contains a binary operator */
  if (strchr("+-*/><=", expr->value[0])) {
    /* If the expression contains a binary operator, evaluate the left and right operands of the operator */
    int left = eval_expression(expr->left);
    int right = eval_expression(expr->right);

    /* Apply the operator to the operands */
    switch (expr->value[0]) {
      case '+':
        value = left + right;
        break;
      case '-':
        value = left - right;
        break;
      case '*':
        value = left * right;
        break;
      case '/':
        value = left / right;
        break;
      case '>':
        value = left > right;
        break;
      case '<':
        value = left < right;
        break;
      case '=':
        value = left == right;
        break;
    }
  }
  else {
    /* If the expression does not contain a binary operator, evaluate it as a term */
    value = eval_term(expr);
  }

  return value;
}

/* Evaluates a term */
int eval_term(tree_node *term) {
  int value = 0;

  /* Check if the term contains a binary operator */
  if (strchr("*/", term->value[0])) {
    /* If the term contains a binary operator, evaluate the left and right operands of the operator */
    int left = eval_term(term->left);
    int right = eval_term(term->right);

    /* Apply the operator to the operands */
    switch (term->value[0]) {
      case '*':
        value = left * right;
        break;
      case '/':
        value = left / right;
        break;
    }
  }
  else {
    /* If the term does not contain a binary operator, evaluate it as a factor */
    value = eval_factor(term);
  }

  return value;
}

/* Evaluates a factor */
int eval_factor(tree_node *factor) {
  int value = 0;

  /* Check if the factor is a variable or a constant */
  if (factor->left) {
    /* If the factor is a variable, return its value */
    value = variables[factor->value[0] - 'a'];
  }
  else {
    /* If the factor is a constant, convert it to an integer and return it */
    value = atoi(factor->value);
  }

  return value;
}

/* Frees the memory occupied by the syntax tree */
void free_syntax_tree(tree_node *root) {
  /* Traverse the left subtree */
  if (root->left) {
    free_syntax_tree(root->left);
  }

  /* Traverse the right subtree */
  if (root->right) {
    free_syntax_tree(root->right);
  }

  /* Free the memory occupied by the current node */
  free(root);
}

int main(int argc, char *argv[]) {
  /* Check if a program was provided as an argument */
  if (argc < 2) {
    printf("Error: No program provided\n");
    return 1;
  }

  /* Parse the program and generate the syntax tree */
  char *program = argv[1];
  tree_node *syntax_tree = parse_program(program);

  /* Print the syntax tree */
  printf("Syntax tree:\n");
  print_syntax_tree(syntax_tree);
  printf("\n");

  /* Evaluate the syntax tree and simulate the execution of the program */
  printf("Output:\n");
  eval_syntax_tree(syntax_tree);

  /* Free the memory occupied by the syntax tree */
  free_syntax_tree(syntax_tree);

  return 0;
}



