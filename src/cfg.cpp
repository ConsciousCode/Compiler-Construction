#include "cfg.hpp"
#include "flow.hpp"
#include <cassert>
#include "ast.hpp"

using namespace cfg;

void CFG::add_edge(CFGBlock* from, CFGBlock* to) {
    if (!from || !to)
        return;
    from->succs.push_back(to);
    to->preds.push_back(from);
}

template<typename C, typename T, typename D>
void for_all(C& container, const T& value, const D& fn) {
    while(true) {
        auto it = std::find(
            container.begin(),
            container.end(),
            value
        );
        if(it == container.end()) break;
        fn(it);
    }
}

void CFG::dce(FlowAnalysis& fa) {
    // Manual iteration because we're updating as we go
    for(size_t off = 0; off < blocks.size(); ++off) {
        const auto& block = blocks[off];
        
        if(block->kind != BlockKind::Assignment) {
            continue;
        }

        auto *pblock = block.get();
        auto *ablock = (AssignmentBlock*)pblock;
        
        // Check for existence just in case
        auto lv_it = fa.lv.find(block->id);
        if(lv_it == fa.lv.end()) {
            continue;
        }

        // Is the variable still live after assignment?
        auto& ana = lv_it->second;
        if(ana.exit.count(ablock->lhs)) {
            continue;
        }

        // It must be dead

        if(entry == pblock) {
            if(block->succs.empty()) {
                // Empty program - is this safe?
                entry = nullptr;
            }
            else {
                assert(block->succs.size() == 1);
                entry = block->succs[0];
            }
        }

        if(block->succs.empty()) {
            // Just remove from preds
            for(const auto& pred : block->preds) {
                for_all(pred->succs, pblock, [&](auto&& it) {
                    pred->succs.erase(it);
                });
            }

            if(entry == pblock) {
                // Is this safe?
                entry = nullptr;
            }

            // Update exit if needed
            if(exit == pblock) {
                if(block->preds.empty()) {
                    // Now an empty program - is this safe?
                    exit = nullptr;
                }
                else {
                    assert(block->preds.size() == 1);
                    exit = block->preds[0];
                }
            }
        }
        else {
            // Assignment should never have more than one successor
            assert(block->succs.size() == 1);

            // Assignment will only ever have one successor
            const auto& succ = block->succs[0];

            // Change pred references to succ
            for(const auto& pred : block->preds) {
                for_all(pred->succs, pblock, [&](auto&& it) {
                    *it = succ;
                });
            }

            // Remove from succ
            for_all(succ->preds, pblock, [&](auto&& it) {
                succ->preds.erase(it);
            });
            // Add preds into succ preds
            succ->preds.insert(
                succ->preds.end(),
                block->preds.begin(),
                block->preds.end()
            );

            if(entry == pblock) {
                entry = block->succs[0];
            }
            if(exit == pblock) {
                if(block->preds.empty()) {
                    // Now an empty program - is this safe?
                    exit = nullptr;
                }
                else {
                    // Assignment should never have more than one pred
                    assert(block->preds.size() == 1);
                    exit = block->preds[0];
                }
            }
        }

        // Remove from flow analysis
        fa.lv.erase(lv_it);

        // Remove from blocks (this will delete it)
        blocks.erase(blocks.begin() + off);

        // Decrement offset to avoid skipping an element
        --off;
    }
}

namespace cfg {

AssignmentBlock::~AssignmentBlock() = default;
ConditionBlock::~ConditionBlock() = default;
SkipBlock::~SkipBlock() = default;

std::string AssignmentBlock::to_c(const ast::CodeGenContext& ctx) const {
    return ctx.indent() + cgen::mangle(lhs) + " = " + ast::to_c(expr, ctx) + ";\n";
}

std::string ConditionBlock::to_c([[maybe_unused]] const ast::CodeGenContext& ctx) const {
    return ""; // Condition is handled in control flow, not as statement
}

std::string SkipBlock::to_c([[maybe_unused]] const ast::CodeGenContext& ctx) const {
    return ""; // Skip does nothing
}

std::string CFG::to_c(const std::string& prog_name, std::set<std::string> vars) const {

    std::string result =
        "#include <stdint.h>\nvoid " + prog_name + "(int64_t vars[]) {\n" + "    int64_t ";

    int index = 0;
    for (const auto& var : vars) {
        if (index > 0)
            result += ", ";
        result += cgen::mangle(var) + " = vars[" + std::to_string(index) + "]";
        ++index;
    }
    result += ";\n";

    ast::CodeGenContext ctx = {1, 0};

    for (const auto& block : blocks) {
        result += "label_" + std::to_string(block->id) + ":\n";
        result += block->to_c(ast::CodeGenContext{1, 0});
        result += generate_control_flow(block.get());
    }

    result += "exit:\n";

    // Store local variables
    index = 0;
    for (const auto& var : vars) {
        result +=
            ctx.indent() + "vars[" + std::to_string(index) + "] = " + cgen::mangle(var) + ";\n";
        ++index;
    }

    result += "}\n";

    return result;
}

std::string CFG::generate_control_flow(CFGBlock* block) const {
    if (block->is_exit) {
        return "    goto exit;\n";
    }

    if (block->kind == BlockKind::Condition) {
        // Condition blocks have 2 successors: true branch, false branch
        if (block->succs.size() == 2) {
            auto* cond = static_cast<ConditionBlock*>(block);
            return "    if (" + ast::to_c(cond->cond, {0, 0}) + ") goto label_" +
                   std::to_string(block->succs[0]->id) + ";\n" + "    else goto label_" +
                   std::to_string(block->succs[1]->id) + ";\n";
        }
    }

    // For assignment/skip, just goto next block
    if (!block->succs.empty()) {
        return "    goto label_" + std::to_string(block->succs[0]->id) + ";\n";
    }

    return "";
}

} // namespace cfg