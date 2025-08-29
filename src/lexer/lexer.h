#include<stdlib.h>
#include<ctype.h>
#include<string>
#include"tokens.h"

typedef struct {
    const char *src;
    size_t pos;
    int line;
} Lexer;

Lexer lexer_new(const char *src){
    Lexer L = {src, 0, 1};
    return L;
}

inline char lexer_peek(Lexer *L){
    return L->src[L->pos];
}

char lexer_next(Lexer *L){
    char c = L->src[L->pos];
    if(c == '\0') return '\0';
    L->pos++;
    if(c == '\n') L->line++;
    return c;
}

Token lexer_next_token(Lexer *L){
    while (isspace((unsigned char) lexer_peek(L))) lexer_next(L);
    
    char c = lexer_peek(L);
    if (c == '\0') return make_token(TOK_EOF, "<eof>", L->line);

    if(isalpha((unsigned char) c) || c == '_'){
        size_t start = L->pos;
        while(isalpha((unsigned char)lexer_peek(L)) || lexer_peek(L) == '_') lexer_next(L);
        size_t len = L->pos - start;
        char *s = strndup(L->src + start, len);
        if(strcmp(s, "func") == 0) { free(s); return make_token(TOK_FN, "func", L->line);}
        if(strcmp(s, "let") == 0) { free(s); return make_token(TOK_LET, "let", L->line);}
        if(strcmp(s, "struct") == 0) { free(s); return make_token(TOK_STRUCT, "struct", L->line);}
        return make_token(TOK_IDENT, s, L-> line);
    }
    if(isdigit((unsigned char)c)){
        size_t start = L->pos;
        while(isdigit((unsigned char)lexer_peek(L))) lexer_next(L);
        
    }
}