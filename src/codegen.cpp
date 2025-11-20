#include "codegen.hpp"

#include "collect_vars.hpp"
#include "emitter.hpp"

#include <functional>
#include <memory>
#include <set>

using namespace ast;

void Codegen::collect_vars(const Block& b) {
    std::set<std::string> S;
    ::collect_vars(b, S);
    vars.assign(S.begin(), S.end());

    // Assigns index for each variable to be used in the offset
    for (std::size_t i = 0; i < vars.size(); i++) {
        slot[vars[i]] = i;
    }
}

CodegenOutput Codegen::generate(const Block& program, std::string program_name) {
    collect_vars(program);

    auto offset_of = [&](const std::string& name) -> long {
        return static_cast<long>(8) * static_cast<long>(slot.at(name));
    };

    Emitter as;

    as.line(".text", false);
    std::string global_name = ".globl " + program_name;
    as.line(global_name, false);
    as.label(program_name);

    std::function<void(const Expr&, const char*)> emit_expr = [&](const Expr& e, const char* dst) {
        std::visit(
            [&](const auto& node) {
                using T = std::decay_t<decltype(node)>;
                if constexpr (std::is_same_v<T, int>) {
                    as.line(std::string("li ") + dst + ", " + std::to_string(node), true);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    as.line(std::string("ld ") + dst + ", " + std::to_string(offset_of(node)) +
                                "(a0)",
                            true);
                } else {
                    emit_expr(node->left, "t0");
                    emit_expr(node->right, "t1");
                    switch (node->op) {
                    case Arith::Op::ADD:
                        as.line(std::string("add ") + dst + ", t0, t1", true);
                        break;
                    case Arith::Op::SUB:
                        as.line(std::string("sub ") + dst + ", t0, t1", true);
                        break;
                    case Arith::Op::MUL:
                        as.line(std::string("mul ") + dst + ", t0, t1", true);
                        break;
                    }
                }
            },
            e);
    };

    std::function<void(const Cond&, const std::string&, const std::string&)> emit_cond =
        [&](const Cond& c, const std::string& Ltrue, const std::string& Lfalse) {
            std::visit(
                [&](const auto& node) {
                    using T = std::decay_t<decltype(node)>;
                    if constexpr (std::is_same_v<T, std::shared_ptr<BoolConst>>) {
                        as.line(std::string("j ") + (node->value ? Ltrue : Lfalse), true);
                    } else if constexpr (std::is_same_v<T, std::shared_ptr<Not>>) {
                        emit_cond(node->cond, Lfalse, Ltrue);
                    } else if constexpr (std::is_same_v<T, std::shared_ptr<Logical>>) {
                        if (node->op == Logical::Op::AND) {
                            auto Lmid = as.gensym("L_and");
                            emit_cond(node->left, Lmid, Lfalse);
                            as.label(Lmid);
                            emit_cond(node->right, Ltrue, Lfalse);
                        } else {
                            auto Lmid = as.gensym("L_or");
                            emit_cond(node->left, Ltrue, Lmid);
                            as.label(Lmid);
                            emit_cond(node->right, Ltrue, Lfalse);
                        }
                    } else {
                        emit_expr(node->left, "t0");
                        emit_expr(node->right, "t1");

                        switch (node->op) {
                        case Comparison::Op::EQ:
                            as.line(std::string("beq t0, t1, ") + Ltrue, true);
                            as.line(std::string("j ") + Lfalse, true);
                            break;
                        case Comparison::Op::LT:
                            as.line(std::string("blt t0, t1, ") + Ltrue, true);
                            as.line(std::string("j ") + Lfalse, true);
                            break;
                        case Comparison::Op::LE:
                            as.line(std::string("bge t1, t0, ") + Ltrue, true);
                            as.line(std::string("j ") + Lfalse, true);
                            break;
                        case Comparison::Op::GT:
                            as.line(std::string("blt t1, t0, ") + Ltrue, true);
                            as.line(std::string("j ") + Lfalse, true);
                            break;
                        case Comparison::Op::GE:
                            as.line(std::string("bge t0, t1, ") + Ltrue, true);
                            as.line(std::string("j ") + Lfalse, true);
                            break;
                        }
                    }
                },
                c);
        };

    std::function<void(const Cmd&)> emit_cmd = [&](const Cmd& c) {
        std::visit(
            [&](const auto& node) {
                using T = std::decay_t<decltype(node)>;
                if constexpr (std::is_same_v<T, Assign>) {
                    emit_expr(node.value, "t0");
                    as.line(std::string("sd t0, ") + std::to_string(offset_of(node.name)) + "(a0)",
                            true);
                } else if constexpr (std::is_same_v<T, If>) {
                    auto Ltrue = as.gensym("L_if_true");
                    auto Lfalse = as.gensym("L_if_false");
                    auto Lend = as.gensym("L_if_end");
                    emit_cond(node.cond, Ltrue, Lfalse);
                    as.label(Ltrue);

                    for (auto& s : node.then.cmds) {
                        emit_cmd(s);
                    }

                    as.line(std::string("j ") + Lend, true);
                    as.label(Lfalse);

                    for (auto& s : node.el.cmds) {
                        emit_cmd(s);
                    }

                    as.label(Lend);
                } else if constexpr (std::is_same_v<T, While>) {
                    auto Lcond = as.gensym("L_while_cond");
                    auto Lbody = as.gensym("L_while_body");
                    auto Lend = as.gensym("L_while_end");

                    as.label(Lcond);
                    emit_cond(node.cond, Lbody, Lend);
                    as.label(Lbody);

                    for (auto& s : node.body.cmds) {
                        emit_cmd(s);
                    }

                    as.line(std::string("j ") + Lcond, true);
                    as.label(Lend);
                } else {
                }
            },
            c);
    };

    for (auto& c : program.cmds) {
        emit_cmd(c);
    }

    as.line("ret", true);

    CodegenOutput out;
    out.riscv = as.str();
    out.vars = vars;
    return out;
}
