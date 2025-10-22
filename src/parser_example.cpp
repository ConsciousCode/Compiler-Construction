#include "parser.hpp"
#include "while_parser.hpp"

int main() {
    parser::ParserGenerator parser = generate_while_parser();

    parser.print_action_table();
    parser.print_goto_table();

    return 0;
}

