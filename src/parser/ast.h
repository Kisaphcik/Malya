typedef enum{
    NODE_FN_DECL,
    NODE_VAR_DECL,
    NODE_BLOCK,
    NODE_RETURN,
    NODE_EXPR_CALL,
    NODE_EXPR_IDENT,
    NODE_EXPR_INT,
    NODE_EXPR_STRING,
} NodeKind;

typedef struct Node {
    NodeKind kind;
    int line;

    char *name; 

    // For fn:
    struct Node **params;
    int param_count;
    char *ret_type;
    struct Node *body; 

    // For var_decl:
    char *var_type; 
    struct Node *init; 

    // For call:
    struct Node **args;
    int arg_count;

    // For literal:
    long int_value;
    char *str_value;
} Node;