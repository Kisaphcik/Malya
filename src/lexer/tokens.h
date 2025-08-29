#include<string.h>

typedef enum{
    TOK_EOF,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_STRING,
    TOK_SEMI,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_COMMA,
    TOK_ARROW,
    TOK_COLON,
    TOK_FN,
    TOK_LET,
    TOK_STRUCT
} TokenKind;

typedef struct{
    TokenKind kind;
    char *lexeme;
    int line;
} Token;

Token make_token(TokenKind k, const char *lexeme, int line){
    Token t;
    t.kind = k;
    t.lexeme = strdup(lexeme);
    t.line = line;
    return t;
}