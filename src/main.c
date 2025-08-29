#include"lexer/lexer.h"

int main() {
    const char *code = "fn add(a: Int, b: Int) -> Int { let x: Int = 5; }";
    Lexer L = lexer_new(code);
    for (;;) {
        Token t = lexer_next_token(&L);
        printf("TOK %d (%s) at line %d\n", t.kind, t.lexeme, t.line);
        if (t.kind == TOK_EOF) break;
    }
    return 0;
}