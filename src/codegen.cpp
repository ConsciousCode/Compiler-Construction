#include "codegen.hpp"
#include "emitter.hpp"

#include <set>
#include <functional>
#include <algorithm>

using namespace ast;

namespace {
    void collect_vars_expr(const Expr& e, std::set<std::string>& S) {
        std::visit([&](const auto& node){
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, int>) {

            } else if constexpr (std::is_same_v<T, std::string>) {
                S.insert(node);
            } else {
                collect_vars_expr(node->left, S);
                collect_vars_expr(node->right, S);
            }
        }, e);
    }

    void collect_vars_cond(const Cond& c, std::set<std::string>& S) {
        std::visit([&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, bool>) {

            } else if constexpr (std::is_same_v<T, std::shared_ptr<Not>>) {
                collect_vars_cond(node->cond, S);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Logical>>) {
                collect_vars_cond(node->left, S);
                collect_vars_cond(node->right, S);
            } else {
                collect_vars_expr(node->left, S);
                collect_vars_expr(node->right, S);
            }
        }, c);
    }

    void collect_vars_cmd(const Cmd& c, std::set<std::string>& S) {
        std::visit([&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, Assign>) {
                S.insert(node.name);
                collect_vars_expr(node.value, S);
            } else if constexpr (std::is_same_v<T, If>) {
                collect_vars_cond(node.cond, S);
                
                for (auto& cc : node.then.cmds) {
                    collect_vars_cmd(cc,S);
                }

                for (auto& cc : node.el.cmds) {
                    collect_vars_cmd(cc, S);
                }
            } else if constexpr (std::is_same_v<T, While>) {
                collect_vars_cond(node.cond, S);

                for (auto& cc : node.body.cmds) {
                    collect_vars_cmd(cc, S);
                }
            } else {}
        }, c);
    }
}

void Codegen::collect_vars(const Block& b) {
    std::set<std::string> S;
    for (auto& c : b.cmds) {
        collect_vars_cmd(c, S);
    }

    vars.assign(S.begin(), S.end());
    std::sort(vars.begin(), vars.end());
    
    for (std::size_t i = 0; i < vars.size(); i++) {
        slot[vars[i]] = i;
    }
}

CodegenOutput Codegen::generate(const Block& program) {
    collect_vars(program);

    auto offset_of = [&](const std::string& name) -> long {
        return static_cast<long>(8) * static_cast<long>(slot.at(name));
    };

    Emitter as;

    as.line(".text");
    as.line(".globl program");
    as.label("program");

    std::function<void(const Expr&, const char*)> emit_expr = [&](const Expr& e, const char* dst) {
        std::visit([&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, int>) {
                as.line(std::string("li ") + dst + ", " + std::to_string(node));
            } else if constexpr (std::is_same_v<T, std::string>) {
                as.line(std::string("ld ") + dst + ", " + std::to_string(offset_of(node)) + "(a0)");
            } else {
                emit_expr(node->left, "t0");
                emit_expr(node->right, "t1");
                switch (node->op) {
                    case Arith::Op::ADD: as.line(std::string("add ") + dst + ", t0, t1"); break;
                    case Arith::Op::SUB: as.line(std::string("sub ") + dst + ", t0, t1"); break;
                    case Arith::Op::MUL: as.line(std::string("mul ") + dst + ", t0, t1"); break;
                }
            }
        }, e);
    };

    std::function<void(const Cond&, const std::string&, const std::string&)> emit_cond = [&](const Cond& c, const std::string& Ltrue, const std::string& Lfalse) {
        std::visit([&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, bool>) {
                as.line(std::string("j ") + (node ? Ltrue : Lfalse));
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

                switch(node->op) {
                    case Comparison::Op::EQ:
                        as.line(std::string("beq t0, t1, ") + Ltrue);
                        as.line(std::string("j ") + Lfalse);
                        break;
                    case Comparison::Op::LT:
                        as.line(std::string("blt t0, t1, ") + Ltrue);
                        as.line(std::string("j ") + Lfalse);
                        break;
                    case Comparison::Op::LE:
                        as.line(std::string("bge t1, t0, ") + Ltrue);
                        as.line(std::string("j ") + Lfalse);
                        break;
                    case Comparison::Op::GT:
                        as.line(std::string("blt t1, t0, ") + Ltrue);
                        as.line(std::string("j ") + Lfalse);
                        break;
                    case Comparison::Op::GE:
                        as.line(std::string("bge t0, t1, ") + Ltrue);
                        as.line(std::string("j ") + Lfalse);
                        break;
                }
            }
        }, c);
    };

    std::function<void(const Cmd&)> emit_cmd = [&](const Cmd& c) {
        std::visit([&](const auto& node){
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, Assign>) {
                emit_expr(node.value, "t0");
                as.line(std::string("sd t0, ") + std::to_string(offset_of(node.name)) + "(a0)");
            } else if constexpr (std::is_same_v<T, If>) {
                auto Ltrue = as.gensym("L_if_true");
                auto Lfalse = as.gensym("L_if_false");
                auto Lend = as.gensym("L_if_end");
                emit_cond(node.cond, Ltrue, Lfalse);
                as.label(Ltrue);
                
                for (auto& s : node.then.cmds) {
                    emit_cmd(s);
                }

                as.line(std::string("j ") + Lend);
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

                as.line(std::string("j ") + Lcond);
                as.label(Lend);
            } else {}
        }, c);
    };

    for (auto& c : program.cmds) {
        emit_cmd(c);
    }

    as.line("ret");

    CodegenOutput out;
    out.riscv = as.str();
    out.vars = vars;
    return out;
}