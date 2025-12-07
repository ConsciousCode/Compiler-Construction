#include "cfg.hpp"
#include "flow.hpp"
#include <cassert>

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

        printf("Dead %s %ld\n", ablock->lhs.c_str(), block->succs.size());

        // It must be dead

        if(block->succs.empty()) {
            // Just remove from preds
            for(const auto& pred : block->preds) {
                for_all(pred->succs, pblock, [&](auto&& it) {
                    pred->succs.erase(it);
                });
                printf("Erase\n");
            }
        }
        else {
            // Assignment will only ever have one successor
            const auto& succ = block->succs[0];

            // Change pred references to succ
            for(const auto& pred : block->preds) {
                for_all(pred->succs, pblock, [&](auto&& it) {
                    *it = succ;
                });
                printf("Assign\n");
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
        }

        // Remove from flow analysis
        fa.lv.erase(lv_it);

        // Remove from blocks (this will delete it)
        blocks.erase(blocks.begin() + off);

        // Decrement offset to avoid skipping an element
        --off;
    }
}