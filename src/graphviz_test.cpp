#include "graphviz.hpp"

#include "parser.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include "while_parser.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

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
        if (t.is_error())
            break;
        tokens.push_back(t);
        t = s.next_token();
    }

    parser::ParserGenerator pg = generate_while_parser();
    parser::Parser p(pg);
    std::shared_ptr<parser::ParseTreeNode> pt = p.parse(tokens);

    graphviz::print_viz_block(*pt->to_ast().get_root());
}
