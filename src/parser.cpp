#include "parser.hpp"

#include "ast.hpp"
#include "token.hpp"
#include "whilelang.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <stack>
#include <string>

namespace parser {

std::shared_ptr<ParseTreeNode> ParseTreeNode::make_nonterminal(whilelang::Nonterm nt, Token t) {
    auto node = std::make_shared<ParseTreeNode>();
    node->is_terminal = false;
    node->nonterminal = nt;
    node->token = std::make_shared<Token>(std::move(t));
    return node;
}

std::shared_ptr<ParseTreeNode> ParseTreeNode::make_terminal(TokenType tt, Token t) {
    auto node = std::make_shared<ParseTreeNode>();
    node->is_terminal = true;
    node->terminal = tt;
    node->token = std::make_shared<Token>(std::move(t));
    return node;
}

std::shared_ptr<ParseTreeNode> Parser::parse(std::vector<Token> const& tokens) {
    std::stack<size_t> state_stack;
    std::stack<std::shared_ptr<ParseTreeNode>> tree_stack;

    state_stack.push(0);
    size_t pos = 0;

    while (true) {
        size_t current_state = state_stack.top();
        Symbol lookahead = (pos < tokens.size()) ? T(tokens[pos].type) : END;

        auto key = std::make_pair(current_state, lookahead);
        auto action_it = pg.action_table.find(key);

        if (action_it == pg.action_table.end()) {
           
            std::cerr << "Parse error at " << tokens[pos].loc.line << ":" << tokens[pos].loc.col
                      << " unexpected token " << tokens[pos].type << std::endl; 
            return nullptr;
        }

        Action action = action_it->second;

        if (action.type == ActionType::SHIFT) {
            auto term_node = ParseTreeNode::make_terminal(tokens[pos].type, tokens[pos]);
            tree_stack.push(term_node);

            state_stack.push(action.value);
            pos++;
        } else if (action.type == ActionType::REDUCE) {
            const Production& prod = pg.get_production(action.value);

            auto nt_node = ParseTreeNode::make_nonterminal(prod.lhs, tokens[pos]);

            // Pop RHS.size smbols from stacks
            size_t rhs_size = prod.rhs.size();
            std::vector<std::shared_ptr<ParseTreeNode>> rhs_nodes;

            for (size_t i = 0; i < rhs_size; i++) {
                state_stack.pop();
                rhs_nodes.push_back(tree_stack.top());
                tree_stack.pop();
            }

            // Reverse for correct order
            std::reverse(rhs_nodes.begin(), rhs_nodes.end());
            nt_node->children = rhs_nodes;

            tree_stack.push(nt_node);

            size_t goto_state = state_stack.top();
            Symbol nt_symbol = N(prod.lhs);
            auto goto_key = std::make_pair(goto_state, nt_symbol);
            auto goto_it = pg.goto_table.find(goto_key);

            if (goto_it == pg.goto_table.end()) {
                std::cerr << "Parse error: no goto" << std::endl;
                return nullptr;
            }

            state_stack.push(goto_it->second);
        } else if (action.type == ActionType::ACCEPT) {
            return tree_stack.top();
        } else {
            std::cerr << "Parse Error" << std::endl;
            return nullptr;
        }
    }
}

void ParseTreeNode::print_tree(std::shared_ptr<ParseTreeNode> node, int indent) {
    if (!node)
        return;
    std::string prefix(indent * 2, ' ');

    if (node->is_terminal) {

        std::cout << prefix << "Terminal: " << node->terminal << "(" << node->token->lexeme << ")"
                  << std::endl;
    } else {
        std::cout << prefix << "Nonterminal: " << node->nonterminal << std::endl;
        for (auto& child : node->children) {
            print_tree(child, indent + 1);
        }
    }
}

ast::Block ParseTreeNode::to_ast_block(std::vector<ast::ASTNode*>& labels) {
    using wnt = whilelang::Nonterm;
    ast::Block blk;

    if (children.size() == 1) {
        // Block -> Cmd (base case)
        if (children[0]->nonterminal == wnt::Cmd) {
            blk.cmds.push_back(children[0]->to_ast_cmd(labels));
        } else {
            std::cerr << "Parse Error: Expected Cmd in Block" << std::endl;
            std::abort();
        }
    } else if (children.size() == 3) {
        // Block -> Block ; Cmd (recursive case)
        if (children[0]->nonterminal == wnt::Block && children[1]->terminal == TokenType::OP_SEMI &&
            children[2]->nonterminal == wnt::Cmd) {

            // Recursively get commands from the first block
            ast::Block rest_block = children[0]->to_ast_block(labels);
            blk.cmds.insert(blk.cmds.end(), rest_block.cmds.begin(), rest_block.cmds.end());

            // Add the current command
            blk.cmds.push_back(children[2]->to_ast_cmd(labels));
        } else {
            std::cerr << "Parse Error: Malformed Block" << std::endl;
            std::abort();
        }
    } else {
        std::cerr << "Parse Error: Unexpected number of children in Block" << std::endl;
        std::abort();
    }

    return blk;
}

ast::Cmd ParseTreeNode::to_ast_cmd(std::vector<ast::ASTNode*>& labels) {
    switch (this->cmd_type()) {
    case TokenType::OP_ASSIGN:{
        size_t label = labels.size();
        auto node = std::make_shared<ast::Assign>(children[0]->token->lexeme, children[2]->to_ast_expr(), label);
        labels.push_back(node.get());
        return ast::Cmd{*node};
    }

    case TokenType::KW_IF:
        return ast::If{children[1]->to_ast_cond(labels), children[3]->to_ast_block(labels),
                       children[5]->to_ast_block(labels)};

    case TokenType::KW_WHILE:
        return ast::While{children[1]->to_ast_cond(labels), children[3]->to_ast_block(labels)};

    case TokenType::KW_SKIP:{
        size_t label = labels.size();
        auto node = std::make_shared<ast::Skip>(label);
        labels.push_back(node.get());
        return ast::Cmd{*node};
    }
    default:
        std::cerr << "Parse error when parsing ast cmd" << std::endl;
        std::abort();
    }
}

ast::Expr ParseTreeNode::to_ast_expr() {
    using wnt = whilelang::Nonterm;

    // Handle terminals (base cases)
    if (is_terminal) {
        if (terminal == TokenType::INTEGER) {
            return int(this->token->value);
        }
        if (terminal == TokenType::IDENT) {
            return this->token->lexeme;
        }
    }

    // Handle nonterminals
    switch (nonterminal) {
    case wnt::ArithExpr:
        if (children.size() == 1) {
            // ArithExpr -> Term
            return children[0]->to_ast_expr();
        } else {
            // ArithExpr -> ArithExpr ArithOp Term
            auto left = children[0]->to_ast_expr();
            auto op = children[1]->get_arith_op();
            auto right = children[2]->to_ast_expr();
            return ast::Arith::make(op, std::move(left), std::move(right));
        }

    case wnt::Term:
        if (children.size() == 1) {
            // Term -> Factor
            return children[0]->to_ast_expr();
        } else {
            // Term -> Term * Factor
            auto left = children[0]->to_ast_expr();
            auto op = ast::Arith::Op::MUL;
            auto right = children[2]->to_ast_expr();
            return ast::Arith::make(op, std::move(left), std::move(right));
        }

    case wnt::Factor:
        if (children.size() == 1) {
            // Factor -> IDENT or INTEGER
            return children[0]->to_ast_expr();
        } else {
            // Factor -> ( ArithExpr )
            return children[1]->to_ast_expr();
        }

    default:
        std::cerr << "Parse error in expression: unexpected nonterminal" << std::endl;
        std::abort();
    }
}

ast::Cond ParseTreeNode::to_ast_cond(std::vector<ast::ASTNode*>& labels) {
    using wnt = whilelang::Nonterm;

    // Handle boolean literals
    if (is_terminal) {
        bool value;
        if (terminal == TokenType::KW_TRUE) {
            value = true;
        }
        else if (terminal == TokenType::KW_FALSE) {
            value = false;
        }
        else {
            std::cerr << "Parse error in condition: unexpected terminal" << std::endl;
            std::abort();
        }

        size_t label = labels.size();
        auto node = std::make_shared<ast::BoolConst>(value, label);
        labels.push_back(node.get());
        return ast::Cond{node};
    }

    switch (nonterminal) {
    case wnt::Cond:
        // Cond -> Disjunction
        return children[0]->to_ast_cond(labels);

    case wnt::Disjunction:
        if (children.size() == 1) {
            // Disjunction -> Conjunction
            return children[0]->to_ast_cond(labels);
        } else {
            // Disjunction -> Disjunction OR Conjunction
            auto left = children[0]->to_ast_cond(labels);
            auto right = children[2]->to_ast_cond(labels);
            
            size_t label = labels.size();
            auto node = std::make_shared<ast::Logical>(ast::Logical::Op::OR, std::move(left), std::move(right), label);
            labels.push_back(node.get());  // Push the raw pointer
            return ast::Cond{node};  // Wrap in variant
        }

    case wnt::Conjunction:
        if (children.size() == 1) {
            // Conjunction -> BoolExpr
            return children[0]->to_ast_cond(labels);
        } else {
            // Conjunction -> Conjunction AND BoolExpr
            auto left = children[0]->to_ast_cond(labels);
            auto right = children[2]->to_ast_cond(labels);

            size_t label = labels.size();
            auto node = std::make_shared<ast::Logical>(ast::Logical::Op::AND, std::move(left), std::move(right), label);
            labels.push_back(node.get());
            return ast::Cond{node};
        }

    case wnt::BoolExpr:
        if (children.size() == 1) {
            // BoolExpr -> TRUE or FALSE
            return children[0]->to_ast_cond(labels);
        } else if (children.size() == 3 && children[1]->nonterminal == wnt::RelOp) {
            // BoolExpr -> ArithExpr RelOp ArithExpr (comparison)
            auto left = children[0]->to_ast_expr();
            auto op = children[1]->get_comparison_op();
            auto right = children[2]->to_ast_expr();

            size_t label = labels.size();
            auto node = std::make_shared<ast::Comparison>(op, std::move(left), std::move(right), label);
            labels.push_back(node.get());
            return ast::Cond{node};
        } else if (children.size() == 3) {
            // BoolExpr -> [ BoolExpr ]
            return children[1]->to_ast_cond(labels);
        } else if (children.size() == 4) {
            // BoolExpr -> NOT [ BoolExpr ]
            auto cond = children[2]->to_ast_cond(labels);
            size_t label = labels.size();
            auto node = std::make_shared<ast::Not>(std::move(cond), label);
            labels.push_back(node.get());
            return ast::Cond{node};
        }
        break;

    default:
        break;
    }

    std::cerr << "Parse error in condition" << std::endl;
    std::abort();
}

ast::Arith::Op ParseTreeNode::get_arith_op() {
    switch (children[0]->terminal) {
    case TokenType::OP_ADD:
        return ast::Arith::Op::ADD;
    case TokenType::OP_SUB:
        return ast::Arith::Op::SUB;
    case TokenType::OP_MUL:
        return ast::Arith::Op::MUL;
    default:
        std::cerr << "Parse Error: Expected arith operation" << std::endl;
        std::abort();
    }
}
ast::Comparison::Op ParseTreeNode::get_comparison_op() {
    switch (children[0]->terminal) {
    case TokenType::OP_EQ:
        return ast::Comparison::Op::EQ;
    case TokenType::OP_GE:
        return ast::Comparison::Op::GE;
    case TokenType::OP_GT:
        return ast::Comparison::Op::GT;
    case TokenType::OP_LE:
        return ast::Comparison::Op::LE;
    case TokenType::OP_LT:
        return ast::Comparison::Op::LT;
    default:
        std::cerr << "Parse Error: Expected Comparison Operation" << std::endl;
        std::abort();
    }
}

/*
 * Gets the command type by looking at the children of a command node.
 */
TokenType ParseTreeNode::cmd_type() const {
    for (std::shared_ptr<ParseTreeNode> child : children) {
        if (child->is_terminal) {
            TokenType t = child->terminal;

            if (t == TokenType::OP_ASSIGN || t == TokenType::KW_WHILE || t == TokenType::KW_IF ||
                t == TokenType::KW_SKIP)
                return t;
        }
    }

    std::cerr << "Parse Error: Expected a command token in child of command" << std::endl;
    std::abort();
}

ast::Logical::Op ParseTreeNode::get_logical_op() {
    switch (terminal) {
    case TokenType::OP_AND:
        return ast::Logical::Op::AND;
    case TokenType::OP_OR:
        return ast::Logical::Op::OR;
    default:
        std::cerr << "Parse Error: Expected Logical Operation" << std::endl;
        std::abort();
    }
}

ast::AST ParseTreeNode::to_ast() {
    std::vector<ast::ASTNode*> labels;
    ast::Block root_block = this->to_ast_block(labels);
    
    return ast::AST{std::make_shared<ast::Block>(std::move(root_block)), std::move(labels)};
}

std::ostream& operator<<(std::ostream& os, const ParseTreeNode& node) {
    if (node.is_terminal) {
        os << "Terminal(" << node.terminal << ", \"" << node.token->lexeme << " ["
           << node.token->value
           << " ]"
              "\")";
    } else {
        os << "Nonterminal(" << node.nonterminal << ")";
    }

    if (!node.children.empty()) {
        os << " [";
        for (size_t i = 0; i < node.children.size(); ++i) {
            if (i > 0)
                os << ", ";
            if (node.children[i]->is_terminal) {
                os << "T:" << node.children[i]->terminal;
            } else {
                os << "NT:" << node.children[i]->nonterminal;
            }
        }
        os << "]";
    }

    return os;
}

} // namespace parser
