#include<stdlib.h>
#include<ctype.h>
#include<string.h>
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

char lexer_peek(Lexer *L){
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
        if(strcmp(s, "return") == 0) { free(s); return make_token(TOK_RETURN, "return", L->line);}

        return make_token(TOK_IDENT, s, L-> line);
    }
    if(isdigit((unsigned char)c)){
        size_t start = L->pos;
        while(isdigit((unsigned char)lexer_peek(L))) lexer_next(L);
        size_t len = L->pos - start;
        char *s = strndup(L->src + start, len);
        return make_token(TOK_NUMBER, s, L->line);
    }

    if(c == '"'){
        lexer_next(L);
        size_t start = L->pos;
        while(lexer_peek(L) != '"' && lexer_peek(L) != '\0') lexer_next(L);
        size_t len = L->pos - start;
        char *s = strndup(L->src + start, len);
        if (lexer_peek(L) == '"') lexer_next(L);
        return make_token(TOK_STRING, s, L->line);
    }

    lexer_next(L);
    switch (c) {
        case ';': return make_token(TOK_SEMI, ";", L->line);
        case '{': return make_token(TOK_LBRACE, "{", L->line);
        case '}': return make_token(TOK_RBRACE, "}", L->line);
        case '(': return make_token(TOK_LPAREN, "(", L->line);
        case '=': return make_token(TOK_EQUAL, "=", L->line);
        case ')': return make_token(TOK_RPAREN, ")", L->line);
        case ',': return make_token(TOK_COMMA, ",", L->line);
        case ':': return make_token(TOK_COLON, ":", L->line);
        case '-':
            if (lexer_peek(L) == '>') { lexer_next(L); return make_token(TOK_ARROW, "->", L->line); }
            break;
    }

    char tbuf[2] = { c, '\0' };
    return make_token(TOK_UNKNOWN, tbuf, L->line);
}