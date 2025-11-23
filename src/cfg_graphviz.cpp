#include "cfg_graphviz.hpp"

#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace {
std::string expr_to_string(const ast::Expr& e);

std::string arith_op_to_string(ast::Arith::Op op) {
    switch (op) {
    case ast::Arith::Op::ADD:
        return "+";
    case ast::Arith::Op::SUB:
        return "-";
    case ast::Arith::Op::MUL:
        return "*";
    }
    return "?";
}

std::string cmp_op_to_string(ast::Comparison::Op op) {
    using Op = ast::Comparison::Op;
    switch (op) {
    case Op::EQ:
        return "=";
    case Op::LT:
        return "<";
    case Op::LE:
        return "<=";
    case Op::GT:
        return ">";
    case Op::GE:
        return ">=";
    }
    return "?";
}

std::string expr_to_string(const ast::Expr& e) {
    return std::visit(
        [&](auto const& v) -> std::string {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                std::string left = expr_to_string(v->left);
                std::string right = expr_to_string(v->right);
                return left + " " + arith_op_to_string(v->op) + " " + right;
            } else {
                static_assert(sizeof(T) == 0, "Unhandled Expr alternative");
            }
        },
        e);
}

std::string cond_to_string(const ast::Cond& c) {
    return std::visit(
        [&](auto const& ptr) -> std::string {
            using PtrT = std::decay_t<decltype(ptr)>;

            if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::BoolConst>>) {
                return ptr->value ? "true" : "false";
            } else if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::Not>>) {
                return "not(" + cond_to_string(ptr->cond) + ")";
            } else if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::Logical>>) {
                std::string op_str = (ptr->op == ast::Logical::Op::AND) ? "and" : "or";
                std::string left = cond_to_string(ptr->left);
                std::string right = cond_to_string(ptr->right);
                return "(" + left + " " + op_str + " " + right + ")";
            } else if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::Comparison>>) {
                std::string left = expr_to_string(ptr->left);
                std::string right = expr_to_string(ptr->right);
                return left + " " + cmp_op_to_string(ptr->op) + " " + right;
            } else {
                static_assert(sizeof(PtrT) == 0, "Unhandled Cond alternative");
            }
        },
        c);
}

void append_used_vars(const cfg::CFGBlock* b, std::ostream& out) {
    const std::vector<std::string>* vars = nullptr;

    if (b->kind == cfg::BlockKind::Assignment) {
        auto const* ab = static_cast<const cfg::AssignmentBlock*>(b);
        vars = &ab->used_vars;
    } else if (b->kind == cfg::BlockKind::Condition) {
        auto const* cb = static_cast<const cfg::ConditionBlock*>(b);
        vars = &cb->used_vars;
    }

    if (!vars || vars->empty()) {
        return;
    }

    out << "\\nvars: ";
    for (std::size_t i = 0; i < vars->size(); i++) {
        if (i > 0)
            out << ", ";
        out << (*vars)[i];
    }
}
} // namespace

namespace cfg {

void print_cfg_dot(const CFG& g, std::ostream& out, bool show_vars) {
    out << "digraph CGF {\n";
    out << " node [shape=box];\n";

    for (const auto& uptr : g.blocks) {
        const CFGBlock* b = uptr.get();
        out << " B" << b->id << " [label=\"";

        if (b->kind == BlockKind::Assignment) {
            const AssignmentBlock* ab = static_cast<const AssignmentBlock*>(b);
            out << ab->lhs << " := " << expr_to_string(ab->expr);
        } else if (b->kind == BlockKind::Condition) {
            const ConditionBlock* cb = static_cast<const ConditionBlock*>(b);
            out << cond_to_string(cb->cond) << " ?";
        } else if (b->kind == BlockKind::Skip) {
            out << "skip";
        }

        if (show_vars) {
            append_used_vars(b, out);
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

        if (b->kind == BlockKind::Condition && b->succs.size() == 2) {
            const CFGBlock* t_succ = b->succs[0];
            const CFGBlock* f_succ = b->succs[1];

            out << " B" << b->id << " -> B" << t_succ->id << " [color=\"green\", label=\"T\"];\n";

            out << " B" << b->id << " -> B" << f_succ->id << " [color=\"red\", label=\"F\"];\n";
        } else {

            for (const CFGBlock* succ : b->succs) {
                out << " B" << b->id << " -> B" << succ->id << ";\n";
            }
        }
    }

    out << "}\n";
}
} // namespace cfg