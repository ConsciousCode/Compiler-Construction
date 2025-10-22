#include <fstream>
#include <iostream>
#include <stdexcept>


#include "scanner.hpp"
#include "parser.hpp"
#include "while_parser.hpp"
#include "ast.hpp"
#include "codegen.hpp"
#include "ast_builder.hpp"

int main() {
    try {
        const std::string path = "examples/add.while";

        Scanner scanner(path.c_str());
        Parser parser = generate_while_parser();
        ParseTreeNode tree = parser.parse(scanner);

        ast::Block program = build_ast_from_parse(tree);

        Codegen cg;
        CodegenOutput out = cg.generate(program);

        std::ofstream ofs("program.s");
        ofs << out.riscv;
        ofs.close();

        std::cerr << "Wrote program.s with " << out.vars.size() << " vars.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: "  << e.what() << "\n";
        return 1;
    }
}