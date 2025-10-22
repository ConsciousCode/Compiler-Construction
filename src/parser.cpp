#include "parser.hpp"
#include "token.hpp"

#include <stack>
#include <memory>
#include <iostream>
#include <string>

namespace parser {

    std::shared_ptr<ParseTreeNode> ParseTreeNode::make_nonterminal(whilelang::Nonterm nt){
        auto node = std::make_shared<ParseTreeNode>();
        node->is_terminal = false;
        node->nonterminal = nt;
        return node;
    }

    std::shared_ptr<ParseTreeNode> ParseTreeNode::make_terminal(TokenType tt, std::string lex){
        auto node = std::make_shared<ParseTreeNode>();
        node->is_terminal = true;
        node->terminal = tt;
        node->lexeme = lex;
        return node;
    }

    std::shared_ptr<ParseTreeNode> Parser::parse(std::vector<Token> const& tokens){
        std::stack<size_t> state_stack;
        std::stack<std::shared_ptr<ParseTreeNode>> tree_stack;

        state_stack.push(0);
        size_t pos = 0;

        while(true){
            size_t current_state = state_stack.top();
            Symbol lookahead = (pos < tokens.size()) ? T(tokens[pos].type) : END;

            auto key = std::make_pair(current_state, lookahead);
            auto action_it = pg.action_table.find(key);
            
            if(action_it == pg.action_table.end()){
                std::cerr << "Parse error: no action for state " << current_state << " and symbol " << (int)lookahead.kind << ":" << lookahead.id << std::endl;
                return nullptr;
            }

            Action action = action_it->second;

            if (action.type == ActionType::SHIFT){
                auto term_node = ParseTreeNode::make_terminal(tokens[pos].type, std::to_string(tokens[pos].value));
                tree_stack.push(term_node);

                state_stack.push(action.value);
                pos++;
            }
            else if (action.type == ActionType::REDUCE) {
                const Production& prod = pg.get_production(action.value);

                auto nt_node = ParseTreeNode::make_nonterminal(prod.lhs);

                // Pop RHS.size smbols from stacks
                size_t rhs_size = prod.rhs.size();
                std::vector<std::shared_ptr<ParseTreeNode>>rhs_nodes;

                for (size_t i = 0; i < rhs_size; i++){
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

                if(goto_it == pg.goto_table.end()) {
                    std::cerr << "Parse error: no goto" << std::endl;
                    return nullptr;
                }
                
                state_stack.push(goto_it->second);
            }
            else if (action.type == ActionType::ACCEPT) {
                return tree_stack.top();
            }
            else {
                std::cerr << "Parse Error" << std::endl;
                return nullptr;
            }
            
        }
    }

    void ParseTreeNode::print_tree(std::shared_ptr<ParseTreeNode> node, int indent) {
        if (!node) return;
        std::string prefix(indent * 2, ' ');

        if(node->is_terminal){
            
            std::cout << prefix << "Terminal: " << node->terminal << "(" << node->lexeme << ")" <<std::endl;
        }
        else {
            std::cout << prefix << "Nonterminal: " << node->nonterminal << std::endl;
            for (auto& child : node->children){
                print_tree(child, indent + 1);
            }
        }
    }

    
}
