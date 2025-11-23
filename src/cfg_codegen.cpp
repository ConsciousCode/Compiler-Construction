#include <algorithm>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "cfg_codegen.hpp"
#include "ast.hpp"
#include "cfg.hpp"
#include "emitter.hpp"
#include "whilegen.hpp"
#include <assert.h>

/**
 * Get the depth of an AST expression to determine the size of the
 * evaluation stack.
**/
int depth_of(const ast::Arith& arith);
int depth_of(const ast::Expr& expr);

int depth_of(const ast::Arith& arith) {
    return std::max(depth_of(arith.left), depth_of(arith.right));
}

int depth_of(const ast::Expr& expr) {
    return std::visit([&](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr(std::is_same_v<T, int>) {
            return 0;
        }
        else if constexpr(std::is_same_v<T, std::string>) {
            return 0;
        }
        else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
            return depth_of(value);
        }
        else {
            static_assert(false, "Non-exhaustive (depth_of)");
        }
    }, expr);
}

/**
 * Stateful meat of codgen from CFG.
**/
struct CFG_Codegen {
    int max_depth = 0;
    std::unordered_map<std::string, int> name_offsets;
    std::unordered_map<size_t, std::string> cond_labels;
    std::string program_name;
    Emitter emit;

    CFG_Codegen(const cfg::CFG& cfg, const std::string& prog) {
        program_name = prog;
        for(const auto& block : cfg.blocks) {
            if(block->kind == cfg::BlockKind::Assignment) {
                auto *ablock = (cfg::AssignmentBlock*)block.get();
                max_depth = std::max(max_depth, depth_of(ablock->expr));
            }
        }
    }

    void emit_line(const std::string& line, bool tab=false) {
        emit.line(line, tab);
    }

    std::string off_reg(int off) {
        return "s" + std::to_string(off + 1);
    }

    std::string name_reg(const std::string& name) {
        return off_reg(name_offsets[name]);
    }

    /**
     * Attempt to build a label for the id. If one already exists, return that
     * instead.
    **/
    std::string build_label(size_t id, const std::string& base) {
        auto it = cond_labels.find(id);
        if(it == cond_labels.end()) {
            auto label = emit.gensym(base);
            cond_labels[id] = label;
            return label;
        }
        else {
            return it->second;
        }
    }

    /**
     * Emit machine code for calculating an arithmetic expression.
     *
     * @param dst The register which should hold the top of the stack, usually t0

    **/
    void emit_arith(const std::string& dst, const ast::Arith& arith, int depth) {
        emit_expr("t0", arith.left, depth);
        emit_expr("t0", arith.right, depth + 1);
        switch(arith.op) {
            case ast::Arith::Op::ADD:
                emit_line("add " + dst + ", t0, t1");
                break;

            case ast::Arith::Op::SUB:
            case ast::Arith::Op::MUL:
                break;
        }
    }

    void emit_expr(const std::string& dst, const ast::Expr& block, int depth) {
        std::visit([&](auto&& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr(std::is_same_v<T, int>) {
                if(depth == 0) {
                    // Load immediate directly into the destination
                    emit_line("ldi " + dst + ", " + std::to_string(node));
                }
                else {
                    // Load immediate into t0 and store
                    emit_line("li t0, " + std::to_string(node));
                    emit_line("sd t0, " + std::to_string(-depth*8) + "(sp)");
                }
            }
            else if constexpr(std::is_same_v<T, std::string>) {
                if(depth == 0) {
                    // Move between registers
                    emit_line("mv " + dst + ", " + name_reg(node));
                }
                else {
                    // Store register onto the stack
                    emit_line("sd " + name_reg(node) + ", " + std::to_string(-depth*8) + "(sp)");
                }
            }
            else if constexpr(std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                if(depth == 0) {
                    // Use the dst directly
                    emit_arith(dst, *node, depth);
                }
                else {
                    // Use t0 as the temporary top of stack and then store
                    emit_arith("t0", *node, depth);
                    emit_line("sd t0, " + std::to_string(-depth*8));
                }
            }
            else {
                static_assert(false, "Non-exhaustive (emit_expr)");
            }
        }, block);
    }

    void emit_cond(const ast::Cond& cond, const std::string& Ltrue, const std::string Lfalse) {
        std::visit([&](auto&& node) {
            using T = typename std::decay_t<decltype(node)>::element_type;
            if constexpr(std::is_same_v<T, ast::BoolConst>) {
                // Immediate jump
                emit_line("j " + (node->value ? Ltrue : Lfalse));
            }
            else if constexpr(std::is_same_v<T, ast::Not>) {
                // Swap the labels
                emit_cond(node, Lfalse, Ltrue);
            }
            else if constexpr(std::is_same_v<T, ast::Logical>) {
                // Short-circuiting
                if (node->op == ast::Logical::Op::AND) {
                    auto Lmid = emit.gensym("L_and");
                    emit_cond(node->left, Lmid, Lfalse);
                    emit.label(Lmid);
                    emit_cond(node->right, Ltrue, Lfalse);
                } else {
                    auto Lmid = emit.gensym("L_or");
                    emit_cond(node->left, Ltrue, Lmid);
                    emit.label(Lmid);
                    emit_cond(node->right, Ltrue, Lfalse);
                }
            }
            else if constexpr(std::is_same_v<T, ast::Comparison>) {
                // Use t2 and t3 to avoid being clobbered by stack evaluation
                emit_expr("t2", node->left, 0);
                emit_expr("t3", node->right, 0);

                switch (node->op) {
                case ast::Comparison::Op::EQ:
                    emit_line("beq t2, t3, " + Ltrue);
                    break;
                case ast::Comparison::Op::LT:
                    emit_line("blt t2, t3, " + Ltrue);
                    break;
                case ast::Comparison::Op::LE:
                    emit_line("bge t3, t2, " + Ltrue);
                    break;
                case ast::Comparison::Op::GT:
                    emit_line("blt t3, t2, " + Ltrue);
                    break;
                case ast::Comparison::Op::GE:
                    emit_line("bge t2, t3, " + Ltrue);
                    break;
                }
                emit_line("j " + Lfalse);
            }
            else {
                static_assert(false, "Non-exhaustive (emit_ast_cond)");
            }
        }, cond);
    }

    void emit_block(const cfg::CFGBlock* block) {
        auto it = cond_labels.find(block->id);
        if(it != cond_labels.end()) {
            emit.label(it->second);
        }
        
        switch(block->kind) {
            case cfg::BlockKind::Assignment: {
                const auto* ablock = (cfg::AssignmentBlock*)block;
                assert(ablock->succs.size() == 1);

                // Put the expr in the register
                emit_expr(name_reg(ablock->lhs), ablock->expr, 0);

                auto *next = ablock->succs[0];

                if(next->preds.size() > 1) {
                    // The next is a target, emit a branch instead
                    emit_line("j " + build_label(next->id, "L_fi"));
                }
                else {
                    // Keep going
                    emit_block(next);
                }
            }

            case cfg::BlockKind::Condition: {
                const auto* cblock = (cfg::ConditionBlock*)block;
                assert(cblock->succs.size() == 2);

                emit_line("#if " + to_while(cblock->cond));                

                auto Lcond = build_label(block->id, "L_loop");
                if(cblock->preds.size() > 1) {
                    emit.label(Lcond);
                }

                auto *bt = cblock->succs[0];
                auto *bf = cblock->succs[1];

                auto Ltrue = build_label(bt->id, "L_if_true");
                auto Lfalse = build_label(bf->id, "L_if_false");
                emit_cond(cblock->cond, Ltrue, Lfalse);
                
                emit_block(bt);
                emit_block(bf);
            }
        }
    }

    void emit_prelude() {
        emit_line(".text", false);
        emit_line(".globl " + program_name, false);
        emit.label(program_name);
        emit_line("addi sp, sp, -" + std::to_string(max_depth*8));

        for(const auto& [name, off] : name_offsets) {
            emit_line("#" + off_reg(off) + "<-input");
            emit_line(
                "ld " + off_reg(off) + ", " + std::to_string(off * 8) + "(a0)"
            );
        }
    }

    void emit_epilogue() {
        emit_line("addi sp, sp, " + std::to_string(max_depth*8));
        for(const auto& [name, off] : name_offsets) {
            emit_line("#output<-" + off_reg(off));
            emit_line(
                "sd " + off_reg(off) + ", " + std::to_string(off * 8) + "(a0)"
            );
        }
        emit_line("ret", false);
    }

    void cfg_codegen(const cfg::CFGBlock* block) {
        emit_prelude();
        emit_block(block);
        emit_epilogue();
    }
};

/**
 * Generate RISC-V pseudocode from a CFG using infinite sN registers.
**/
std::string cfg::cfg_codegen(const cfg::CFG& cfg, const std::string& program_name) {
    CFG_Codegen cgen(cfg, program_name);
    cgen.cfg_codegen(cfg.entry);
    return cgen.emit.str();
}