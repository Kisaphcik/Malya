#include"parser/parser_debug.h"

int main() {
    const char *code =
        "func add(a: Int, b: Int) -> Int {\n"
        "  let x: Int = 5;\n"
        "  return x;\n"
        "}\n"
        "\n"
        "func main() -> Void {\n"
        "  add(1, 2);\n"
        "}\n";

    Parser p;
    parser_init(&p, code);

    int count = 0;
    Node **prog = parse_program(&p, &count);
    printf("Parsed %d top-level items\n\n", count);
    for (int i=0;i<count;i++) {
        print_node(prog[i], 0);
        printf("\n");
    }

    return 0;
}