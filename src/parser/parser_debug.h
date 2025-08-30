void print_indent(int n) {
    for (int i=0;i<n;i++) putchar(' ');
}

void print_node(Node *n, int indent) {
    if (!n) return;
    switch (n->kind) {
        case NODE_FN_DECL:
            print_indent(indent); printf("FnDecl %s -> %s (line %d)\n", n->name, n->ret_type, n->line);
            for (int i=0;i<n->param_count;i++) {
                print_indent(indent+2); printf("Param:\n");
                Node *p = n->params[i];
                print_indent(indent+4); printf("name=%s type=%s\n", p->name, p->var_type);
            }
            print_indent(indent+2); printf("Body:\n");
            print_node(n->body, indent+4);
            break;
        case NODE_BLOCK:
            print_indent(indent); printf("Block (stmts=%d)\n", n->param_count);
            for (int i=0;i<n->param_count;i++) print_node(n->params[i], indent+2);
            break;
        case NODE_VAR_DECL:
            print_indent(indent); printf("VarDecl %s : %s\n", n->name, n->var_type);
            if (n->init) { print_indent(indent+2); printf("Init:\n"); print_node(n->init, indent+4); }
            break;
        case NODE_RETURN:
            print_indent(indent); printf("Return:\n");
            print_node(n->init, indent+2);
            break;
        case NODE_EXPR_CALL:
            print_indent(indent); printf("Call %s (args=%d)\n", n->name, n->arg_count);
            for (int i=0;i<n->arg_count;i++) print_node(n->args[i], indent+2);
            break;
        case NODE_EXPR_IDENT:
            print_indent(indent); printf("Ident %s\n", n->name);
            break;
        case NODE_EXPR_INT:
            print_indent(indent); printf("Int %lld\n", (long long)n->int_value);
            break;
        case NODE_EXPR_STRING:
            print_indent(indent); printf("String \"%s\"\n", n->str_value);
            break;
        default:
            print_indent(indent); printf("Unknown node kind %d\n", n->kind);
    }
}