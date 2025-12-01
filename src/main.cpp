#include "ast.hpp"
#include "cfg_builder.hpp"
#include "cfg_codegen.hpp"
#include "cfg_graphviz.hpp"
#include "codegen.hpp"
#include "graphviz.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"

#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

enum class C_GEN_SOURCE { AST, CFG };

std::map<std::string, C_GEN_SOURCE> cgen_source_map{{"ast", C_GEN_SOURCE::AST},
                                                    {"cfg", C_GEN_SOURCE::CFG}};

C_GEN_SOURCE stringToCGenSource(const std::string& str) {
    auto it = cgen_source_map.find(str);
    if (it != cgen_source_map.end()) {
        return it->second;
    }
    throw std::runtime_error("Invalid C Generation Source");
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser prog("wisc");

    /* Core Compiler Args */
    prog.add_argument("input").help("Input file");
    prog.add_argument("-o", "--output").help("Output file for Assembly").default_value("out.s");
    prog.add_argument("-cc", "--c-code").help("Generate equivalent c code").flag();
    prog.add_argument("--c-source")
        .help("C code generation source")
        .default_value(std::string{"ast"})
        .choices("ast", "cfg");

    /* Print Scanner Tokens*/
    prog.add_argument("-pt", "--print-tokens")
        .help("Print the token stream from the scanner")
        .flag();

    /* Parse Tree */
    prog.add_argument("-ppt", "--print-parse-tree").help("Print the parse tree").flag();
    prog.add_argument("-gpt", "--graph-parse-tree")
        .help("Graph the parse tree to a dot file.")
        .default_value(std::string("parse_tree.dot"));

    /* AST Output */
    prog.add_argument("-pa", "--print-ast").help("Print the abstract syntax tree").flag();
    prog.add_argument("-ga", "--graph-ast")
        .help("Graph the abstract syntax tree to a dot file.")
        .default_value(std::string("ast.dot"));

    /* CFG Output */
    prog.add_argument("-gc", "--graph-cfg")
        .help("Graph the control flow graph to a dot file.")
        .default_value(std::string("cfg.dot"));
    prog.add_argument("-sv", "--show-cfg-vars").help("Include used vars in CFG node labels").flag();

    try {
        prog.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    /* Extract Arguments */
    std::filesystem::path input_path(prog.get<std::string>("input"));
    std::string program_name = input_path.stem().string();
    std::filesystem::path output_path(prog.get<std::string>("--output"));

    const bool GEN_C_CODE = prog.is_used("--c-code");
    const C_GEN_SOURCE C_SOURCE(stringToCGenSource(prog.get<std::string>("--c-source")));

    const bool PRINT_TOKENS = prog.get<bool>("--print-tokens");

    const bool PRINT_PARSE_TREE = prog.get<bool>("--print-parse-tree");
    const bool GRAPH_PARSE_TREE = prog.is_used("--graph-parse-tree");
    std::filesystem::path parse_tree_graph_path(prog.get<std::string>("--graph-parse-tree"));

    const bool PRINT_AST = prog.get<bool>("--print-ast");
    const bool GRAPH_AST = prog.is_used("--graph-ast");
    std::filesystem::path ast_graph_path(prog.get<std::string>("--graph-ast"));

    const bool GRAPH_CFG = prog.is_used("--graph-cfg");
    std::filesystem::path cfg_graph_path(prog.get<std::string>("--graph-cfg"));
    const bool SHOW_CFG_VARS = prog.get<bool>("--show-cfg-vars");

    /* Load and read input file */
    std::ifstream input_file(input_path);
    if (!input_file) {
        std::cerr << "Failed to open input file: " << input_path << "\n";
        return 1;
    }
    std::string code, line;
    while (std::getline(input_file, line)) {
        code += line + "\n";
    }
    input_file.close();

    /* Scan the input */
    std::vector<Token> tokens;
    Scanner s = Scanner(code.c_str());
    Token t = s.next_token();
    while (t.type != TokenType::ERROR_EOF) {
        if (PRINT_TOKENS) {
            std::cout << t << std::endl;
        }
        if (t.is_error()) {
            break;
        }
        tokens.push_back(t);
        t = s.next_token();
    }
    if (PRINT_TOKENS) {
        printf("\n");
    }

    /* Parse the tokens */
    parser::ParserGenerator pg = generate_while_parser();
    parser::Parser p(pg);
    std::shared_ptr<parser::ParseTreeNode> pt = p.parse(tokens);

    if (!pt) {
        std::cerr << "Parse failed.\n";
        return 1;
    }
    if (PRINT_PARSE_TREE) {
        // TODO: Print to arbitrary output, not just stdout
        parser::ParseTreeNode::print_tree(pt);
        printf("\n");
    }
    if (GRAPH_PARSE_TREE) {
        // TODO: Print to stdout
        parser::write_dot_file(pt, parse_tree_graph_path.string());
    }

    /* Build the AST */
    ast::AST ast(nullptr, {});
    try {
        ast = pt->to_ast();
    } catch (const std::exception& e) {
        std::cerr << "AST construction failed: " << e.what() << "\n";
        return 1;
    }
    ast::Block& program = *ast.get_root();

    if (PRINT_AST) {
        fprintf(stderr, "Print AST not implemented\n");
    }
    if (GRAPH_AST) {
        std::ofstream ast_dot_file(ast_graph_path);
        if (!ast_dot_file) {
            std::cerr << "Failed to open AST output file: " << ast_graph_path << "\n";
            return 1;
        }

        std::streambuf* oldbuf = std::cout.rdbuf(ast_dot_file.rdbuf());
        graphviz::print_viz_block(program);
        std::cout.rdbuf(oldbuf);
        ast_dot_file.close();
    }

    /* Build the CFG */
    cfg::CFG g;
    try {
        g = cfg::build_cfg(program);
    } catch (const std::exception& e) {
        std::cerr << "CFG construction failed: " << e.what() << "\n";
        return 1;
    }

    if (GRAPH_CFG) {
        std::ofstream cfg_dot_file(cfg_graph_path);

        if (!cfg_dot_file) {
            std::cerr << "Failed to open CFG output file: " << cfg_graph_path << "\n";
            return 1;
        }

        cfg::print_cfg_dot(g, cfg_dot_file, SHOW_CFG_VARS);
        cfg_dot_file.close();
    }

    /* Generate Code */
    /*
    Codegen cg;
    CodegenOutput out_code;
    try {
        out_code = cg.generate(program, program_name);
    } catch (const std::exception& e) {
        std::cerr << "Code generation failed: " << e.what() << "\n";
        return 1;
    }
    */

    if (GEN_C_CODE) {
        std::ofstream c_output_file(output_path);

        switch (C_SOURCE) {
        case C_GEN_SOURCE::AST: {
            c_output_file << ast.to_c(program_name);
            c_output_file.close();
            break;
        }
        case C_GEN_SOURCE::CFG: {
            throw std::logic_error("C Generation from CFG not yet implemented");
            break;
        }
        default:
            throw std::runtime_error("Attempting to build equivalent c from unknown source");
        }

        c_output_file.close();
    } else {
        std::ofstream output_file(output_path);

        if (!output_file) {
            std::cerr << "Failed to open assembly output file: " << output_path << "\n";
            return 1;
        }

        // output_file << out_code.riscv;
        output_file << cfg::cfg_codegen(g, program_name);
        output_file.close();
    }

    return 0;
}
