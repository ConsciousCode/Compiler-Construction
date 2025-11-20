#include "ast.hpp"
#include "codegen.hpp"
#include "graphviz.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"
#include "cfg_builder.hpp"
#include "cfg_graphviz.hpp"

#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum Stage { ERROR, TOKENIZE, PARSE, AST, CODEGEN };

void flag(argparse::ArgumentParser& prog, const char* name, const char* help) {
    prog.add_argument(name).help(help).default_value(false).implicit_value(true);
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser prog("wisc");

    prog.add_argument("input").help("Input file");
    flag(prog, "--tokens", "Print the token stream");
    flag(prog, "--parse-tree", "Print the parse tree");
    flag(prog, "--graph-parse-tree", "Graph the parse tree");
    flag(prog, "--ast", "Print the abstract syntax tree");
    flag(prog, "--graph-ast", "Graph the abstract syntax tree");
    flag(prog, "--asm", "Print the assembly code");
    flag(prog, "--build-cfg", "Build and print the control flow graph");
    prog.add_argument("-o", "--output").help("Output file").default_value("-").required();

    try {
        prog.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    bool print_tokens = prog["--tokens"] == true, 
         print_pt = prog["--parse-tree"] == true,
         graph_pt = prog["--graph-parse-tree"] == true, 
         print_ast = prog["--ast"] == true,
         graph_ast = prog["--graph-ast"] == true, 
         print_asm = prog["--asm"] == true,
         build_cfg = prog["--build-cfg"] == true;

    unsigned actions = print_tokens << TOKENIZE 
                        | (print_pt || graph_pt) << PARSE 
                        |(print_ast || graph_ast) << AST 
                        | print_asm << CODEGEN;

    if (actions == 0) {
        actions = -1;
    }

    // Load in program name
    std::filesystem::path input_path(prog.get<std::string>("input"));
    std::string program_name = input_path.stem().string();

    std::string outfn = prog.get<std::string>("--output");
    std::ofstream ofs;
    std::ostream* out = &std::cout;

    if (outfn != "-") {
        ofs.open(outfn);
        if (!ofs) {
            std::cerr << "Failed to open output file: " << outfn << "\n";
            return 1;
        }
        out = &ofs;
    }

    std::ostream& outf = *out;

    // Load and read file;
    std::ifstream ifs(input_path);

    if (!ifs) {
        std::cerr << "Failed to open input file: " << input_path << "\n";
        return 1;
    }

    std::string code, line;
    while (std::getline(ifs, line)) {
        code += line + "\n";
    }
    ifs.close();

    std::vector<Token> tokens;
    Scanner s = Scanner(code.c_str());
    Token t = s.next_token();
    while (t.type != TokenType::ERROR_EOF) {
        if (print_tokens) {
            outf << t << std::endl;
        }
        if (t.is_error())
            break;
        tokens.push_back(t);
        t = s.next_token();
    }

    if (actions < 1 << PARSE) {
        return 0;
    }

    parser::ParserGenerator pg = generate_while_parser();
    parser::Parser p(pg);
    std::shared_ptr<parser::ParseTreeNode> pt = p.parse(tokens);

    if (print_pt) {
        // TODO: Print to arbitrary output, not just stdout
        parser::ParseTreeNode::print_tree(pt);
    }
    if (graph_pt) {
        // TODO: Print to stdout
        parser::write_dot_file(pt, outfn);
    }

    if (actions < 1 << AST) {
        return 0;
    }

    ast::AST ast = pt->to_ast();
    ast::Block& program = *ast.get_root();

    if (print_ast) {
        // ???
        fprintf(stderr, "Print AST not implemented\n");
    }
    if (graph_ast) {
        // TODO: Print to arbitrary output
        graphviz::print_viz_block(program);
    }

    if (actions < 1 << CODEGEN) {
        return 0;
    }

    //CFG Management
    if (build_cfg) {
        cfg::CFG g = cfg::build_cfg(program);

        cfg::print_cfg_dot(g, outf);
    }

    Codegen cg;
    CodegenOutput out_code = cg.generate(program, program_name);

    outf << out_code.riscv;

    return 0;
}
