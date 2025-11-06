#include "ast_examples.hpp"

#include "ast.hpp"

using namespace std::string_literals;
using namespace ast;

Block make_example_ast_factorial() {

    Block program{
        Assign{"y", "x", 0},
        Assign{"z", 1, 0},
        While{Comparison::make(Comparison::Op::GT, std::string{"y"}, 1, 0),
              Block{Assign{"z", Arith::make(Arith::Op::MUL, "z", "y"), 0},
                    Assign{"y", Arith::make(Arith::Op::SUB, "y", 1), 0}}},
        Assign{"y", 0, 0},
        Assign{"output", "z", 0},
    };

    return program;
}
