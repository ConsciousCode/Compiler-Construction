#include "ast.hpp"
#include "codegen.hpp"
#include "graphviz.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"
#include "cfg_builder.hpp"
#include "cfg_graphviz.hpp"
#include "cfg_codegen.hpp"

#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


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
    flag(prog, "--show-cfg-vars", "Include used vars in CFG node labels");

    prog.add_argument("-o", "--output")
        .help("Output file")
        .default_value("-")
        .required();

    prog.add_argument("--output-asmb")
        .help("Assembly output file")
        .default_value(std::string("-"));
    
    prog.add_argument("--output-asp")
        .help("Ast graph (Graphviz DOT) output file")
        .default_value(std::string("-"));

    prog.add_argument("--output-cfg")
        .help("CFG graph (Graphviz DOT) output file")
        .default_value(std::string("-"));

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
         build_cfg = prog["--build-cfg"] == true,
         show_cfg_vars = prog["--show-cfg-vars"] == true;

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

    std::string asmb_outfn = prog.get<std::string>("--output-asmb");
    std::string asp_outfn = prog.get<std::string>("--output-asp");
    std::string cfg_outfn = prog.get<std::string>("--output-cfg");

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
        if (t.is_error()) {
            break;
        }
        tokens.push_back(t);
        t = s.next_token();
    }

    parser::ParserGenerator pg = generate_while_parser();
    parser::Parser p(pg);
    std::shared_ptr<parser::ParseTreeNode> pt = p.parse(tokens);

    if (!pt) {
        std::cerr << "Parse failed.\n";
        return 1;
    }

    if (print_pt) {
        // TODO: Print to arbitrary output, not just stdout
        parser::ParseTreeNode::print_tree(pt);
    }
    if (graph_pt) {
        // TODO: Print to stdout
        parser::write_dot_file(pt, outfn);
    }

    ast::AST ast(nullptr, {});
    try {
        ast = pt->to_ast();
    } catch (const std::exception& e) {
        std::cerr << "AST construction failed: " << e.what() << "\n";
        return 1;
    }
    ast::Block& program = *ast.get_root();

    if (print_ast) {
        // ???
        fprintf(stderr, "Print AST not implemented\n");
    }
    if (graph_ast) {
        // TODO: Print to arbitrary output
        if (asp_outfn == "-") {
            graphviz::print_viz_block(program);
        } else {
            std::ofstream ast_ofs(asp_outfn);
            if (!ast_ofs) {
                std::cerr << "Failed to open AST output file: " << asp_outfn << "\n";
                return 1;
            }

            std::streambuf* oldbuf = std::cout.rdbuf(ast_ofs.rdbuf());
            graphviz::print_viz_block(program);
            std::cout.rdbuf(oldbuf);
        }
    }

    cfg::CFG g;
    try {
        g = cfg::build_cfg(program);
    } catch (const std::exception& e) {
        std::cerr << "CFG construction failed: " << e.what() << "\n";
        return 1;
    }

    //CFG Management
    if (build_cfg) {
        if (cfg_outfn == "-") {
            cfg::print_cfg_dot(g, std::cout, show_cfg_vars);
        } else {
            std::ofstream cfg_ofs(cfg_outfn);

            if (!cfg_ofs) {
                std::cerr << "Failed to open CFG output file: " << cfg_outfn << "\n";
                return 1;
            }

            cfg::print_cfg_dot(g, cfg_ofs, show_cfg_vars);
        }
    }

    /*
    if (print_asm) {
        Codegen cg;
        CodegenOutput out_code;
        try {
            out_code = cg.generate(program, program_name);
        } catch (const std::exception& e) {
            std::cerr << "Code generation failed: " << e.what() << "\n";
            return 1;
        }

        if (asmb_outfn == "-") {
            outf << out_code.riscv;
        } else {
            std::ofstream asmb_ofs(asmb_outfn);

            if (!asmb_ofs) {
                std::cerr << "Failed to open assembly output file: " << asmb_outfn << "\n";
                return 1;
            }

            asmb_ofs << out_code.riscv;
        }
    }
    */
    if(print_asm) {
        std::cout << cfg::cfg_codegen(g, "program");
    }

    return 0;
}
