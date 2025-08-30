#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include"../lexer/lexer.h"
#include"ast.h"

typedef struct{
    Lexer lexer;
    Token cur;
} Parser;

void parser_init(Parser *p, const char *src);
Token parser_next_token(Parser *p);
void parser_advance(Parser *p);
int parser_match(Parser *p, TokenKind k);
int parser_expect(Parser *p, TokenKind k, const char *errmsg);

void panic_at(Token* t, const char *msg){
    fprintf(stderr, "[Parse Error] line %d: %s (near '%s')\n", t->line, msg, t->lexeme);
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

char *parse_type_name(Parser *p) {
    if (p->cur.kind == TOK_IDENT) {
        char *tname = strdup(p->cur.lexeme);
        parser_advance(p);
        return tname;
    } else {
        panic_at(&p->cur, "Expected type name");
        return NULL;
    }
}

char *parse_ident(Parser *p) {
    if (p->cur.kind == TOK_IDENT) {
        char *r = strdup(p->cur.lexeme);
        parser_advance(p);
        return r;
    } else {
        panic_at(&p->cur, "Expected identifier");
        return NULL;
    }
}

Node *parse_expr(Parser *p);

Node *parse_primary(Parser *p) {
    if (p->cur.kind == TOK_NUMBER) {
        Node *n = node_new(NODE_EXPR_INT, p->cur.line);
        n->int_value = atoll(p->cur.lexeme);
        parser_advance(p);
        return n;
    }
    if (p->cur.kind == TOK_STRING) {
        Node *n = node_new(NODE_EXPR_STRING, p->cur.line);
        n->str_value = strdup(p->cur.lexeme);
        parser_advance(p);
        return n;
    }
    if (p->cur.kind == TOK_IDENT) {
        char *name = strdup(p->cur.lexeme);
        parser_advance(p);
        if (p->cur.kind == TOK_LPAREN) {
            parser_advance(p); 
            Node *call = node_new(NODE_EXPR_CALL, p->cur.line);
            call->name = name;
            call->args = NULL;
            call->arg_count = 0;
            if (p->cur.kind != TOK_RPAREN) {
                while (1) {
                    Node *arg = parse_expr(p);
                    call->args = realloc(call->args, sizeof(Node*) * (call->arg_count + 1));
                    call->args[call->arg_count++] = arg;
                    if (p->cur.kind == TOK_COMMA) { parser_advance(p); continue; }
                    break;
                }
            }
            parser_expect(p, TOK_RPAREN, "Expected ')' after call args");
            return call;
        } else {
            Node *n = node_new(NODE_EXPR_IDENT, p->cur.line);
            n->name = name;
            return n;
        }
    }
    panic_at(&p->cur, "Unexpected token in expression");
    return NULL;
}

Node *parse_expr(Parser *p) {
    return parse_primary(p);
}

Node *parse_statement(Parser *p) {
    if (p->cur.kind == TOK_LET) {
        parser_advance(p);
        Node *n = node_new(NODE_VAR_DECL, p->cur.line);
        n->name = parse_ident(p);
        parser_expect(p, TOK_COLON, "Expected ':' after variable name");
        n->var_type = parse_type_name(p);
        parser_expect(p, TOK_EQUAL, "Expected '=' after type in let");
        n->init = parse_expr(p);
        parser_expect(p, TOK_SEMI, "Expected ';' after let declaration");
        return n;
    }
    if (p->cur.kind == TOK_RETURN) {
        parser_advance(p);
        Node *n = node_new(NODE_RETURN, p->cur.line);
        n->init = parse_expr(p);
        parser_expect(p, TOK_SEMI, "Expected ';' after return");
        return n;
    }
    Node *e = parse_expr(p);
    parser_expect(p, TOK_SEMI, "Expected ';' after expression");
    return e;
}

Node *parse_block(Parser *p) {
    parser_expect(p, TOK_LBRACE, "Expected '{' to start block");
    Node *blk = node_new(NODE_BLOCK, p->cur.line);
    blk->params = NULL;
    blk->param_count = 0;
    while (p->cur.kind != TOK_RBRACE && p->cur.kind != TOK_EOF) {
        Node *stmt = parse_statement(p);
        blk->params = realloc(blk->params, sizeof(Node*) * (blk->param_count + 1));
        blk->params[blk->param_count++] = stmt;
    }
    parser_expect(p, TOK_RBRACE, "Expected '}' at end of block");
    return blk;
}

Node *parse_param(Parser *p) {
    Node *param = node_new(NODE_VAR_DECL, p->cur.line);
    param->name = parse_ident(p);
    parser_expect(p, TOK_COLON, "Expected ':' in parameter");
    param->var_type = parse_type_name(p);
    return param;
}

void parse_param_list(Parser *p, Node *fn_node) {
    parser_expect(p, TOK_LPAREN, "Expected '(' for parameter list");
    fn_node->params = NULL;
    fn_node->param_count = 0;
    if (p->cur.kind != TOK_RPAREN) {
        while (1) {
            Node *param = parse_param(p);
            fn_node->params = realloc(fn_node->params, sizeof(Node*) * (fn_node->param_count + 1));
            fn_node->params[fn_node->param_count++] = param;
            if (p->cur.kind == TOK_COMMA) { parser_advance(p); continue; }
            break;
        }
    }
    parser_expect(p, TOK_RPAREN, "Expected ')' after parameters");
}

Node *parse_function(Parser *p) {
    int line = p->cur.line;
    parser_expect(p, TOK_FN, "Expected 'func' keyword");
    char *fname = parse_ident(p);
    Node *fn = node_new(NODE_FN_DECL, line);
    fn->name = fname;
    parse_param_list(p, fn);
    if (p->cur.kind == TOK_ARROW) {
        parser_advance(p);
        fn->ret_type = parse_type_name(p);
    } else {
        fn->ret_type = strdup("void"); 
    }
    fn->body = parse_block(p);
    return fn;
}

Node **parse_program(Parser *p, int *out_count) {
    Node **items = NULL;
    int count = 0;
    while (p->cur.kind != TOK_EOF) {
        if (p->cur.kind == TOK_FN) {
            Node *f = parse_function(p);
            items = realloc(items, sizeof(Node*)*(count+1));
            items[count++] = f;
            continue;
        } else {
            panic_at(&p->cur, "Unexpected top-level token (expected 'func')");
        }
    }
    *out_count = count;
    return items;
}