#include "cfg_graphviz.hpp"

#include <string>
#include <variant>
#include <memory>
#include <type_traits>
#include <ostream>


namespace {
    std::string expr_to_string(const ast::Expr& e);

    std::string arith_op_to_string(ast::Arith::Op op) {
        switch (op) {
            case ast::Arith::Op::ADD: return "+";
            case ast::Arith::Op::SUB: return "-";
            case ast::Arith::Op::MUL: return "*";
        }
        return "?";
    }

    std::string expr_to_string(const ast::Expr& e) {
        return std::visit(
            [&](auto const& v) -> std::string{
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, int>) {
                    return std::to_string(v);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return v;
                } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                    if (!v) return "<null>";

                    std::string left = expr_to_string(v->left);
                    std::string right = expr_to_string(v->right);
                    return left + " " + arith_op_to_string(v->op) + " " + right;
                } else {
                    static_assert(sizeof(T) == 0, "Unhandled Expr alternative");
                }
            },
            e
        );
    }
}

namespace cfg {
    void print_cfg_dot(const CFG& g, std::ostream& out) {
        out << "digraph CGF {\n";
        out << " node [shape=box];\n";

        for (const auto& uptr : g.blocks) {
            const CFGBlock* b = uptr.get();
            out << " B" << b->id << " [label=\"";

            if (b->kind == BlockKind::Assignment) {
                const AssignmentBlock* ab = static_cast<const AssignmentBlock*>(b);
                out << ab->lhs << " := " << expr_to_string(ab->expr);
            } else {
                out << "block " << b->id;
            }

            out << "\"";

            if (b->is_entry) {
                out << ", style=bold";
            }
            if (b->is_exit) {
                out << ", peripheries=2";
            }

            out << "];\n";
        }

        for (const auto& uptr : g.blocks) {
            const CFGBlock* b = uptr.get();
            for (const CFGBlock* succ : b->succs) {
                out << " B" << b->id << " -> B" << succ->id << ";\n";
            }
        }

        out << "}\n";
    }
}