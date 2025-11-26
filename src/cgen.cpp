/**
 * Generate C code from the AST.
 */

#include "ast.hpp"
#include "collect_vars.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <variant>

/**
 * https://stackoverflow.com/questions/2896600/#answer-24315631
 */
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string indent(std::string str) {
    return "    " + ReplaceAll(str, "\n", "\n    ");
}

std::string mangle(std::string var) {
    var = ReplaceAll(var, "_", "__");
    var = ReplaceAll(var, "'", "_");
    return var;
}

std::string to_c_inner(const ast::Arith& arith);
std::string to_c_inner(const ast::Cond& cond);
std::string to_c_inner(const ast::Assign& assign);
std::string to_c_inner(const ast::Block& block);

std::string to_c_inner(const ast::Expr& expr) {
    std::stringstream code;
    std::visit(
        [&code](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                code << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                code << mangle(arg);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                code << to_c_inner(*arg);
            }
        },
        expr);
    return code.str();
}

std::string to_c_inner(const ast::Arith& arith) {
    std::stringstream code;
    code << "(" << to_c_inner(arith.left) << " ";
    switch (arith.op) {
    case ast::Arith::Op::ADD:
        code << "+";
        break;
    case ast::Arith::Op::SUB:
        code << "-";
        break;
    case ast::Arith::Op::MUL:
        code << "*";
        break;
    }
    code << " " << to_c_inner(arith.right) << ")";
    return code.str();
}

std::string to_c_inner(const ast::Assign& assign) {
    std::stringstream code;
    code << mangle(assign.name) << " = " << to_c_inner(assign.value) << ";\n";
    return code.str();
}

std::string to_c_inner(const ast::Cond& cond) {
    std::stringstream code;
    std::visit(
        [&code](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<ast::BoolConst>>) {
                code << (arg->value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Not>>) {
                code << "!(" << to_c_inner(arg->cond) << ")";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Logical>>) {
                code << "(" << to_c_inner(arg->left) << " ";
                switch (arg->op) {
                case ast::Logical::Op::AND:
                    code << "&& ";
                    break;
                case ast::Logical::Op::OR:
                    code << "|| ";
                    break;
                }
                code << to_c_inner(arg->right) << ")";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Comparison>>) {
                code << "(" << to_c_inner(arg->left) << " ";
                switch (arg->op) {
                case ast::Comparison::Op::EQ:
                    code << "== ";
                    break;
                case ast::Comparison::Op::LT:
                    code << "< ";
                    break;
                case ast::Comparison::Op::LE:
                    code << "<= ";
                    break;
                case ast::Comparison::Op::GT:
                    code << "> ";
                    break;
                case ast::Comparison::Op::GE:
                    code << ">= ";
                    break;
                }
                code << to_c_inner(arg->right) << ")";
            }
        },
        cond);
    return code.str();
}

std::string to_c_inner(const ast::Block& block) {
    bool first = true;
    std::stringstream code;
    for (auto cmd : block.cmds) {
        if (!first)
            code << "\n";
        first = false;
        std::visit(
            [&code](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, ast::Assign>) {
                    code << mangle(arg.name) << " = " << to_c_inner(arg.value) << ";";
                } else if constexpr (std::is_same_v<T, ast::If>) {
                    code << "if (" << to_c_inner(arg.cond) << ") {\n";
                    code << indent(to_c_inner(arg.then));
                    code << "\n}\n";
                    code << "else {\n";
                    code << indent(to_c_inner(arg.el));
                    code << "\n}";
                } else if constexpr (std::is_same_v<T, ast::While>) {
                    code << "while (" << to_c_inner(arg.cond) << ") {\n";
                    code << indent(to_c_inner(arg.body));
                    code << "\n}";
                } else if constexpr (std::is_same_v<T, ast::Skip>) {
                    code << ";";
                } else {
                    throw std::runtime_error("Unsupported command encountered");
                }
            },
            cmd);
    }
    return code.str();
}

std::string to_c(std::string entry, const ast::Block& block) {
    std::set<std::string> vars;
    collect_vars(block, vars);

    std::stringstream code;
    code << "#include <stdint.h>\n";
    code << "void " << entry << "(int64_t vars[]) {\n";

    // Load local variables
    code << "    int64_t ";

    int index = 0;
    for (const auto& var : vars) {
        if (index > 0)
            code << ", ";
        code << mangle(var) << " = vars[" << index << "]";
        ++index;
    }
    code << ";\n";

    // Code
    code << indent(to_c_inner(block)) << "\n";

    // Store local variables
    index = 0;
    for (const auto& var : vars) {
        code << "    vars[" << index << "] = " << mangle(var) << ";\n";
        ++index;
    }

    code << "}\n";
    return code.str();
}

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
    std::shared_ptr<parser::ParseTreeNode> parse_tree = p.parse(tokens);

    ast::AST ast = parse_tree->to_ast();

    std::cout << to_c(program_name, *ast.get_root());
}
