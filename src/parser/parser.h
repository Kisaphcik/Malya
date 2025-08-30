#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include"lexer/lexer.h"
#include"asr.h"

typedef struct{
    Lexer lexer;
    Token cur
} Parser;

void parser_init(Parser *p, const char *src);
Token parser_next_token(Parser *p);
void parser_advance(Parser *p);
int parser_match(Parser *p, TokenKind k);
int parser_expect(Parser *p, TokenKind k, const char *errmsg);

void panic_at(Token* t, const char *msg){
    fprint(stderr, "[Parse Error] line %d: %s (near '%s')\n", t->line, msg, t->lexeme)
    exit(1);
}

void parser_init(Parser *p, const char *src) {
    p->lexer = lexer_new(src);
    p->cur = lexer_next_token(&p->lexer);
}

Token parser_next_token(Parser *p) {
    Token t = lexer_next_token(&p->lexer);
    return t;
}

void parser_advance(Parser *p) {
    free(p->cur.lexeme);
    p->cur = parser_next_token(p);
}

int parser_match(Parser *p, TokenKind k) {
    return p->cur.kind == k;
}

int parser_expect(Parser *p, TokenKind k, const char *errmsg) {
    if (p->cur.kind == k) {
        parser_advance(p);
        return 1;
    } else {
        panic_at(&p->cur, errmsg);
        return 0;
    }
}
