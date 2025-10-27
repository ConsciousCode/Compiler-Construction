#include "ast.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"
#include "codegen.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    // Load in program name
    std::filesystem::path input_path(argv[1]);
    std::string program_name = input_path.stem().string();

    // Load and read file;
    std::ifstream ifs(argv[1]);
    std::string prog, line;
    while (std::getline(ifs, line)) {
        prog += line + "\n";
    }
    ifs.close();

    std::vector<Token> tokens;

    Scanner s = Scanner(prog.c_str());
    Token t = s.next_token();
    while (t.type != TokenType::ERROR_EOF) {
        std::cout << t;
        if (t.is_error())
            break;
        tokens.push_back(t);
        t = s.next_token();
    }

    parser::ParserGenerator pg = generate_while_parser();

    parser::Parser p(pg);

    std::shared_ptr<parser::ParseTreeNode> parse_tree = p.parse(tokens);
    parser::ParseTreeNode::print_tree(parse_tree);

    ast::Block program = parse_tree->to_ast_block();
    
    Codegen cg;
    CodegenOutput out = cg.generate(program, program_name);

    std::string output_file = program_name + ".s";
    std::ofstream ofs(output_file);
    ofs << out.riscv;
    std::cerr << "Wrote " + output_file + " with " << out.vars.size() << " vars.\n";

    return 0;
}
