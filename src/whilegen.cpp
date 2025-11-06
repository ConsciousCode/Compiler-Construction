/**
 * Generate While code from the AST.
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <variant>

#include "ast.hpp"
#include "scanner.hpp"
#include "while_parser.hpp"

/**
 * https://stackoverflow.com/questions/2896600/#answer-24315631
 */
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string indent(std::string str) {
    return "    " + ReplaceAll(str, "\n", "\n    ");
}

int collect_label(const ast::Cond& cond) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr(std::is_same_v<T, std::shared_ptr<ast::Not>>) {
            return arg->get_label();
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Logical>>) {
            return arg->get_label();
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Comparison>>) {
            return arg->get_label();
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::BoolConst>>) {
            return arg->get_label();
        }
        else {
            throw std::runtime_error("Unsupported condition");
        }
    }, cond);
}

std::string to_while(const ast::Arith& arith);
std::string to_while(const ast::Cond& cond);
std::string to_while(const ast::Assign& assign);
std::string to_while(const ast::Block& block);

std::string to_while(const ast::Expr& expr) {
    std::stringstream code;
    std::visit([&code](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr(std::is_same_v<T, int>) {
            code << arg;
        }
        else if constexpr(std::is_same_v<T, std::string>) {
            code << arg;
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
            code << to_while(*arg);
        }
    }, expr);
    return code.str();
}

std::string to_while(const ast::Arith& arith) {
    std::stringstream code;
    code << "(" << to_while(arith.left) << " ";
    switch(arith.op) {
        case ast::Arith::Op::ADD: code << "+"; break;
        case ast::Arith::Op::SUB: code << "-"; break;
        case ast::Arith::Op::MUL: code << "*"; break;
    }
    code << " " << to_while(arith.right) << ")";
    return code.str();
}

std::string to_while(const ast::Assign& assign) {
    std::stringstream code;
    code << assign.name << " := " << to_while(assign.value) << ";\n";
    return code.str();
}

std::string to_while(const ast::Cond& cond) {
    std::stringstream code;
    std::visit([&code](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr(std::is_same_v<T, std::shared_ptr<ast::BoolConst>>) {
            code << (arg->value? "true" : "false");
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Not>>) {
            code << "not [" << to_while(arg->cond) << "]";
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Logical>>) {
            code << "[" << to_while(arg->left) << " ";
            switch(arg->op) {
                case ast::Logical::Op::AND:
                    code << "and ";
                    break;
                case ast::Logical::Op::OR:
                    code << "or ";
                    break;
            }
            code << to_while(arg->right) << "]";
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Comparison>>) {
            code << "[" << to_while(arg->left) << " ";
            switch(arg->op) {
                case ast::Comparison::Op::EQ:
                    code << "= ";
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
            code << to_while(arg->right) << "]";
        }
    }, cond);
    return code.str();
}

std::string to_while(const ast::Block& block) {
    bool first = true;
    std::stringstream code;
    for(auto cmd : block.cmds) {
        if(!first) code << "\n";
        first = false;
        std::visit([&code](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<T, ast::Assign>) {
                code << arg.name << " := " << to_while(arg.value);
                code << " {- " << arg.get_label() << " -}";
            }
            else if constexpr(std::is_same_v<T, ast::If>) {
                code << "if " << to_while(arg.cond) << " then";
                code << " {- " << collect_label(arg.cond) << " -}\n";
                code << indent(to_while(arg.then));
                code << "\nelse\n";
                code << indent(to_while(arg.el));
                code << "\nfi";
            }
            else if constexpr(std::is_same_v<T, ast::While>) {
                code << "while " << to_while(arg.cond) << " do";
                code << " {- " << collect_label(arg.cond) << " -}\n";
                code << indent(to_while(arg.body));
                code << "\nod";
            }
            else if constexpr(std::is_same_v<T, ast::Skip>) {
                code << "skip";
                code << " {- " << arg.get_label() << " -}";
            }
            else {
                throw std::runtime_error("Unsupported command");
            }
        }, cmd);
    }
    return code.str();
}

int main(int argc, char *argv[]) {
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
    
    std::cout << to_while(*pt->to_ast().get_root());
}
