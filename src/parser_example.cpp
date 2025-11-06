#include "ast.hpp"
#include "graphviz.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

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
        std::cout << t << std::endl;
        if (t.is_error())
            break;
        tokens.push_back(t);
        t = s.next_token();
    }

    parser::ParserGenerator pg = generate_while_parser();

    parser::Parser p(pg);

    std::shared_ptr<parser::ParseTreeNode> parse_tree = p.parse(tokens);
    parser::ParseTreeNode::print_tree(parse_tree);

    if (parse_tree) {
        parser::write_dot_file(parse_tree, "parse_tree.dot");
    }

    ast::AST ast = parse_tree->to_ast();
    graphviz::print_viz_block(*ast.get_root());
    return 0;
}
