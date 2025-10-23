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
        std::vector<Token> tokens;

        for (Token tok = scanner.next_token(); tok.type != TokenType::ERROR_EOF && !tok.is_error(); tok = scanner.next_token()) {
            tokens.push_back(tok);
        }

        parser::Parser parser = generate_while_parser();
        std::shared_ptr<parser::ParseTreeNode> tree = parser.parse(tokens);

        ast::Block program = build_ast_from_parse(*tree);

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