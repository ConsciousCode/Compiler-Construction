
#include "collect_vars.hpp"

using namespace ast;

void collect_vars(const Expr& e, std::set<std::string>& vars) {
    // int has no variable names -> left out
    std::visit(
        [&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, std::string>) {
                vars.insert(node);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Arith>>) {
                collect_vars(node->left, vars);
                collect_vars(node->right, vars);
            }
        },
        e);
}

void collect_vars(const Cond& c, std::set<std::string>& vars) {
    // Bool has no variable names -> left out
    std::visit(
        [&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Not>>) {
                collect_vars(node->cond, vars);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Logical>>) {
                collect_vars(node->left, vars);
                collect_vars(node->right, vars);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Comparison>>) {
                collect_vars(node->left, vars);
                collect_vars(node->right, vars);
            }
        },
        c);
}

void collect_vars(const Cmd& c, std::set<std::string>& vars) {
    std::visit(
        [&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, Assign>) {
                // Add assignment variable name Ex: x
                vars.insert(node.name);

                // Collect variables from the expression following the assignment
                collect_vars(node.value, vars);
            } else if constexpr (std::is_same_v<T, If>) {
                // Collect variables from condition
                collect_vars(node.cond, vars);

                // Collect variables from all the commands inside if block
                for (auto& cc : node.then.cmds) {
                    collect_vars(cc, vars);
                }

                // Collect variables from all commands inside else block
                for (auto& cc : node.el.cmds) {
                    collect_vars(cc, vars);
                }
            } else if constexpr (std::is_same_v<T, While>) {
                // Collect variables from condition of while loop
                collect_vars(node.cond, vars);

                // Collect all unique variables from inside while loop body
                for (auto& cc : node.body.cmds) {
                    collect_vars(cc, vars);
                }
            }
        },
        c);
}

void collect_vars(const Block& b, std::set<std::string>& vars) {
    // Collect all the unique variables in the ast
    for (auto& c : b.cmds) {
        collect_vars(c, vars);
    }
}
