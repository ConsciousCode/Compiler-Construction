#include "graphviz.hpp"

#include "ast_examples.hpp"

void test_graphviz_factorial() {
    auto ast = make_example_ast_factorial();
    graphviz::print_viz_block(ast);
}

int main() {
    test_graphviz_factorial();
    return 0;
}
