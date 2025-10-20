#include "ast_examples.hpp"
#include "ast.hpp"

using namespace std::string_literals;
using namespace ast;

Block make_example_ast_factorial(){
	
	Block program {
		Assign{"y", "x"},
		Assign{"z", 1},
		While{
			Comparison::make(Comparison::Op::GT,std::string{"y"},1),
			Block{
				Assign{"z", Arith::make(Arith::Op::MUL, "z", "y")},
				Assign{"y", Arith::make(Arith::Op::SUB, "y", 1)}
			}
		},
		Assign{"y", 0},
		Assign{"output", "z"},
	};
	

	return program;
}
