#include "cfg_codegen.hpp"

#include "ast.hpp"
#include "cfg.hpp"
#include "collect_vars.hpp"
#include "emitter.hpp"
#include "whilegen.hpp"

#include <algorithm>
#include <cassert>
#include <set>
#include <string>
#include <type_traits>
#include <map>

/**
 * Get the depth of an AST expression to determine the size of the
 * evaluation stack.
 **/
static Allocation g_alloc;
int depth_of(const ast::Arith& arith);
int depth_of(const ast::Expr& expr);

int depth_of(const ast::Arith& arith) {
    return std::max(depth_of(arith.left), depth_of(arith.right)) + 1;
}

int depth_of(const ast::Expr& expr) {
    return std::visit(
        [&](auto&& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, int>) {
                return 0;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return 0;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                return depth_of(*node);
            } else {
                static_assert(!sizeof(T*), "Non-exhaustive (depth_of)");
            }
        },
        expr);
}

static std::string var_reg(const std::string& name) {
    auto it = g_alloc.var_to_reg.find(name);
    if (it == g_alloc.var_to_reg.end()) {
        return "";
    }

    return it->second;
}


static int var_slot(const std::string& name) {
    auto it = g_alloc.var_to_slot.find(name);
    assert(it != g_alloc.var_to_slot.end());
    return it->second;
}

static std::string var_addr(const std::string& name) {
    int slot = var_slot(name);
    int offset = slot * 8;
    return std::to_string(offset) + "(a0)";
}
/**
 * Stateful meat of codgen from CFG.
 **/
struct CFG_Codegen {
    int max_depth;
    std::map<std::string, int> name_offsets;
    std::map<size_t, std::string> cond_labels;
    std::set<size_t> seen_ids;
    std::string program_name;
    Emitter emit;

    CFG_Codegen(const cfg::CFG& cfg, const std::string& prog){
        std::set<std::string> varset;
        program_name = prog;
        max_depth = 0;

        // Aggregate all vars and the maximum stack depth for expression evaluation (not including spilled vars)

        for (const auto& block : cfg.blocks) {
            switch (block->kind) {
            case cfg::BlockKind::Assignment: {
                auto* ablock = (cfg::AssignmentBlock*)block.get();
                max_depth = std::max(max_depth, depth_of(ablock->expr));

                varset.insert(ablock->lhs);
                varset.insert(ablock->used_vars.begin(), ablock->used_vars.end());
                break;
            }
            case cfg::BlockKind::Condition: {
                auto* cblock = (cfg::ConditionBlock*)block.get();
                collect_vars(cblock->cond, varset);
                break;
            }
            case cfg::BlockKind::Skip: {
                break;
            }
            }
        }

        // Construct var offsets in the array

        int off = 0;
        for (const auto& var : varset) {
            name_offsets[var] = off++;
        }

        name_offsets.clear();
        for (const auto& [name, slot] : g_alloc.var_to_slot) {
            name_offsets[name] = static_cast<int>(slot);
        }
    }

    void emit_line(const std::string& line, bool tab = true) {
        emit.line(line, tab);
        // if(tab) std::cout << "   ";
        // std::cout << line << std::endl;
    }

    void emit_comm(const std::string& comment, bool tab = true) {
        emit_line("# " + comment, tab);
    }

    void emit_label(const std::string& label) {
        emit.label(label);
        // std::cout << label << ":" << std::endl;
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
        if (it == cond_labels.end()) {
            auto label = base + "_" + std::to_string(id);
            cond_labels[id] = label;
            return label;
        } else {
            return it->second;
        }
    }

    std::string build_block_label(const cfg::CFGBlock* block) {
        auto it = cond_labels.find(block->id);
        if (it == cond_labels.end()) {
            std::string base;
            if (block->kind == cfg::BlockKind::Condition) {
                base = "L_cond";
            } else {
                base = "L_block";
            }
            auto label = base + "_" + std::to_string(block->id);
            cond_labels[block->id] = label;
            return label;
        } else {
            return it->second;
        }
    }

    /**
     * @return Whether or not the node has emitted code.
     **/
    bool seen(const cfg::CFGBlock* block) {
        if (block == nullptr)
            return false;
        return seen_ids.count(block->id) != 0;
    }

    // Invariant for the eval stack: no subexpression can use areas of the stack
    // below the current depth.

    /**
     * Emit machine code for calculating an arithmetic expression.
     *
     * Arith can never be a bare variable so it doesn't return its dst.
     *
     * @param dst The register which should hold the top of the stack, usually t0

    **/
    void emit_arith(const std::string& dst, const ast::Arith& arith, int depth) {
        auto lhs = emit_expr("t0", arith.left, depth);
        auto rhs = emit_expr("t1", arith.right, depth + 1);
        switch (arith.op) {
        case ast::Arith::Op::ADD:
            emit_asm("add", dst, lhs, rhs);
            break;

        case ast::Arith::Op::SUB:
            emit_asm("sub", dst, lhs, rhs);
            break;
        case ast::Arith::Op::MUL:
            emit_asm("mul", dst, lhs, rhs);
            break;
        }
    }

    void emit_asm(const std::string& op, const std::string& a) {
        emit_line(op + " " + a);
    }

    void emit_asm(const std::string& op, const std::string& a, const std::string& b) {
        emit_line(op + " " + a + ", " + b);
    }

    void emit_asm(const std::string& op, const std::string& a, const std::string& b,
                  const std::string& c) {
        emit_line(op + " " + a + ", " + b + ", " + c);
    }

    std::string deref(int by, const std::string& reg) {
        return std::to_string(by) + "(" + reg + ")";
    }

    /**
     * Emit machine code for calculating an expression.
     *
     * @param dst  The register which should hold the top of the stack if a location is not already available.
     *
     * @return The actual location the result is in
     **/
    std::string emit_expr(const std::string& dst, const ast::Expr& block, int depth) {
        return std::visit(
            [&](auto&& node) {
                using T = std::decay_t<decltype(node)>;
                if constexpr (std::is_same_v<T, int>) {
                    // Load immediate into t0 and store
                    emit_asm("li", dst, std::to_string(node));
                } else if constexpr (std::is_same_v<T, std::string>) {
                    // Return either the register or the default dst
                    std::string r = var_reg(node);
                    return r.empty()? dst : r;
                } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                    // Use t0 as the temporary top of stack and then store
                    emit_arith(dst, *node, depth);
                } else {
                    static_assert(!sizeof(T*), "Non-exhaustive (emit_expr)");
                }
                return dst;
            },
            block
        );
    }

    /**
     * Emit machine code for a conditional node (if/while) using the given labels.
     **/
    void emit_cond(const ast::Cond& cond, const std::string& Ltrue, const std::string Lfalse) {
        std::visit(
            [&](auto&& node) {
                using T = typename std::decay_t<decltype(node)>::element_type;
                if constexpr (std::is_same_v<T, ast::BoolConst>) {
                    // Immediate jump
                    emit_asm("j", node->value ? Ltrue : Lfalse);
                } else if constexpr (std::is_same_v<T, ast::Not>) {
                    // Swap the labels
                    emit_cond(node->cond, Lfalse, Ltrue);
                } else if constexpr (std::is_same_v<T, ast::Logical>) {
                    // Short-circuiting
                    if (node->op == ast::Logical::Op::AND) {
                        auto Lmid = emit.gensym("L_and");
                        emit_cond(node->left, Lmid, Lfalse);
                        emit_label(Lmid);
                        emit_cond(node->right, Ltrue, Lfalse);
                    } else {
                        auto Lmid = emit.gensym("L_or");
                        emit_cond(node->left, Ltrue, Lmid);
                        emit_label(Lmid);
                        emit_cond(node->right, Ltrue, Lfalse);
                    }
                } else if constexpr (std::is_same_v<T, ast::Comparison>) {
                    // We can use these temporaries instead because relational
                    // operator expressions can't nest.
                    auto lhs = emit_expr("t2", node->left, 0);
                    auto rhs = emit_expr("t3", node->right, 1);

                    // RISC-V only has beq, blt, and bge but with these we can derive
                    // <= and > by swapping the order of operands

                    switch (node->op) {
                    case ast::Comparison::Op::EQ:
                        emit_asm("beq", lhs, rhs, Ltrue);
                        break;
                    case ast::Comparison::Op::LT:
                        emit_asm("blt", lhs, rhs, Ltrue);
                        break;
                    case ast::Comparison::Op::LE:
                        emit_asm("bge", rhs, lhs, Ltrue); // Swap
                        break;
                    case ast::Comparison::Op::GT:
                        emit_asm("blt", rhs, lhs, Ltrue); // Swap
                        break;
                    case ast::Comparison::Op::GE:
                        emit_asm("bge", lhs, rhs, Ltrue);
                        break;
                    }
                    emit_asm("j", Lfalse);
                } else {
                    static_assert(!sizeof(T*), "Non-exhaustive (emit_ast_cond)");
                }
            },
            cond);
    }

    /**
     * Emit machine code for a generic block in the CFG. This emits using
     * depth-first search where any block with more than one predecessor gets
     * a label.
     *
     * @return The successor, the entry of a basic block
     **/
    void emit_block(const cfg::CFGBlock* block) {
        // Continue until we've seen the block
        if (block == nullptr) {
            // Just in case?
            return;
        }
        if (seen(block)) {
            emit_asm("j", cond_labels[block->id]);
            return;
        }
        seen_ids.insert(block->id);

        switch (block->kind) {
        case cfg::BlockKind::Assignment: {
            const auto* ablock = (cfg::AssignmentBlock*)block;

            if (ablock->preds.size() > 1 || cond_labels.count(ablock->id) > 0) {
                emit_label(build_label(ablock->id, "L_block"));
            }

            // Put the expr in the register
            emit_comm(ablock->lhs + " := " + to_while(ablock->expr));
            std::string lhs = var_reg(ablock->lhs);
            if (lhs.empty()) {
                // Not a register, must move onto stack
                auto rhs = emit_expr("t0", ablock->expr, 0);
                emit_asm("sd", rhs, var_addr(ablock->lhs));
            } else {
                auto rhs = emit_expr(lhs, ablock->expr, 0);
                // x := y
                if(rhs != lhs) {
                    emit_asm("mv", lhs, rhs);
                }
            }

            if (ablock->succs.size() == 0) {
                emit_asm("j", "L_return");
            } else {
                emit_block(ablock->succs[0]);
            }
            break;
        }

        case cfg::BlockKind::Condition: {
            const auto* cblock = (cfg::ConditionBlock*)block;
            // Still pretty sure this is right at least...
            assert(cblock->succs.size() > 0);

            emit_comm("if " + to_while(cblock->cond), false);

            emit_label(build_label(block->id, "L_cond"));

            cfg::CFGBlock *bt = cblock->succs[0], *bf = nullptr;
            std::string Ltrue = build_label(bt->id, "L_true"), Lfalse = "L_return";
            if (cblock->succs.size() > 1) {
                bf = cblock->succs[1];
                Lfalse = build_label(bf->id, "L_false");
            }

            emit_cond(cblock->cond, Ltrue, Lfalse);

            if (!seen(bt)) {
                emit_comm("then", false);
                emit_block(bt);
            }

            if (bf && !seen(bf)) {
                emit_comm("else", false);
                emit_block(bf);
            }
            break;
        }

        case cfg::BlockKind::Skip: {
            emit_comm("Skip");
            auto it = cond_labels.find(block->id);
            if (it != cond_labels.end()) {
                emit_label(it->second);
            }

            if (block->succs.size() == 0) {
                emit_asm("j", "L_return");
            } else {
                emit_block(block->succs[0]);
            }
            break;
        }
        }
    }

    void emit_prologue() {
        std::string sb;
        ;
        for (const auto& [name, off] : name_offsets) {
            std::string r = var_reg(name);
            if (!r.empty()) {
                sb = name + "=" + r + "(slot " + std::to_string(off) + "), " + sb;
            } else {
                sb = name + "=[spill @slot " + std::to_string(off) + "], " + sb;
            }
        }

        emit_line("#vars: " + sb, false);

        emit_line(".text", false);
        emit_line(".globl " + program_name, false);
        emit_label(program_name);

        // Allocate stack space for the expression evaluation stack
        emit_line("addi sp, sp, -" + std::to_string(max_depth * 8));

        // Unload variables from the array
        for (const auto& [name, off] : name_offsets) {
            std::string r = var_reg(name);
            if (!r.empty()) {
                emit_comm(r + " <- input (" + name + ")");
                emit_line("ld " + r + ", " + std::to_string(off * 8) + "(a0)");
            }
        }
        emit_line("");
    }

    void emit_epilogue() {
        emit_line("");

        emit_label("L_return");

        auto out = name_offsets.find("output");
        if(out != name_offsets.end()) {
            std::string reg = var_reg("output");
            if(reg.empty()) {
                emit_asm("ld", "a0", var_addr("output"));
            }
            else {
                emit_asm("mv", "a0", reg);
            }
        }

        // Undo the stack allocation
        emit_line("addi sp, sp, " + std::to_string(max_depth * 8));

        // Load variables back into the array
        emit_line("ret");
    }

    void cfg_codegen(const cfg::CFGBlock* block) {
        emit_prologue();
        emit_block(block);
        emit_epilogue();
    }
};

/**
 * Generate RISC-V pseudocode from a CFG using infinite sN registers.
 **/
std::string cfg::cfg_codegen(const cfg::CFG& cfg, const std::string& program_name, const Allocation& alloc) {
    g_alloc = alloc;
    CFG_Codegen cgen(cfg, program_name);
    cgen.cfg_codegen(cfg.entry);
    return cgen.emit.str();
}
