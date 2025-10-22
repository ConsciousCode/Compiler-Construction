// Code in this file was generated with the help of Anthropic's Claude LLM.

#include "parser.hpp"
#include "whilelang.hpp"

#include <iostream>
#include <fstream>

namespace parser {
    
    int DotGenerator::generate_node(std::shared_ptr<ParseTreeNode> node) {
        if (!node) return -1;
        
        int current_id = node_counter++;
        
        if (node->is_terminal) {
            // Terminal nodes - use box shape and different color
            dot << "  node" << current_id << " [label=\"" 
                << node->terminal;
            if (!node->lexeme.empty()) {
                dot << "\\n'" << escape_dot(node->lexeme) << "'";
            }
            dot << "\", shape=box, style=filled, fillcolor=lightblue];\n";
        } else {
            // Nonterminal nodes - use ellipse shape
            dot << "  node" << current_id << " [label=\"" 
                << whilelang::to_string(node->nonterminal) 
                << "\", shape=ellipse, style=filled, fillcolor=lightgreen];\n";
            
            // Add edges to children
            for (auto& child : node->children) {
                int child_id = generate_node(child);
                if (child_id >= 0) {
                    dot << "  node" << current_id << " -> node" << child_id << ";\n";
                }
            }
        }
        
        return current_id;
    }
    
    std::string DotGenerator::escape_dot(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '"' || c == '\\') {
                result += '\\';
            }
            result += c;
        }
        return result;
    }
    
    std::string DotGenerator::generate(std::shared_ptr<ParseTreeNode> root) {
        node_counter = 0;
        dot.str("");
        dot.clear();
        
        dot << "digraph ParseTree {\n";
        dot << "  node [fontname=\"Arial\"];\n";
        dot << "  edge [fontname=\"Arial\"];\n";
        
        if (root) {
            generate_node(root);
        }
        
        dot << "}\n";
        return dot.str();
    }
    
    void write_dot_file(std::shared_ptr<ParseTreeNode> root, const std::string& filename) {
        DotGenerator gen;
        std::string dot_content = gen.generate(root);
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << dot_content;
            file.close();
            std::cout << "DOT file written to: " << filename << std::endl;
        } else {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
    }
}
