#include "flow.hpp"
#include "cfg.hpp"
#include <algorithm>
#include <iterator>

/**
 * Flow analysis equation, its use depends on the type of analysis being
 * done. All of them are of the form V(l) = V'(l) \ kill U gen.
 * Analyses differ in which V and V' are (entry vs exit) and if they're
 * combined with union or intersection.
**/
struct AnaEq {
    std::unordered_set<AnaTerm> gen;
    std::unordered_set<AnaTerm> kill;
};

void lv_genset(cfg::CFGBlock& block, std::unordered_set<AnaTerm>& gen) {
    switch(block.kind) {
        case cfg::BlockKind::Assignment: {
            auto *ablock = (cfg::AssignmentBlock*)&block;
            for(const auto& fv : ablock->used_vars) {
                gen.insert(AnaTerm{fv, block.id});
            }
        }
        
        case cfg::BlockKind::Condition: {
            auto *cblock = (cfg::ConditionBlock*)&block;
            for(const auto& fv : cblock->used_vars) {
                gen.insert(AnaTerm{fv, block.id});
            }
        }
        case cfg::BlockKind::Skip:
            break;
    }
}

void lv_killset(cfg::CFGBlock& block, std::unordered_set<AnaTerm>& kill) {
    switch(block.kind) {
        case cfg::BlockKind::Assignment: {
            auto *ablock = (cfg::AssignmentBlock*)&block;
            kill.insert(AnaTerm{ablock->lhs, block.id});
        }
        
        case cfg::BlockKind::Condition:
        case cfg::BlockKind::Skip:
            break;
    }
}

Analysis flow_analysis(const cfg::CFG& cfg) {
    // Build equations
    std::unordered_map<size_t, AnaEq> lv_eqns;

    for(const auto& block : cfg.blocks) {
        AnaEq lv_eqn;
        lv_genset(*block, lv_eqn.gen);
        lv_killset(*block, lv_eqn.kill);
        lv_eqns[block->id] = lv_eqn;
    }

    // Fixpoint equation solving
    Analysis ana; // Initialize to empty
    bool fixpoint;
    do {
        // true -> false whenever any set is changed
        fixpoint = true;
        for(const auto& block : cfg.blocks) {
            auto& lv_eqn = lv_eqns[block->id];
            auto& lv_ana = ana.lv[block->id];

            /// entry = exit U gen \ kill ///
            auto orig = lv_ana.entry;
            
            // tmp = exit U gen
            std::unordered_set<AnaTerm> tmp;
            std::set_union(
                lv_ana.exit.begin(), lv_ana.exit.end(),
                lv_eqn.gen.begin(), lv_eqn.gen.end(),
                std::inserter(tmp, tmp.begin())
            );

            // entry = tmp \ kill
            lv_ana.entry.clear();
            std::set_difference(
                tmp.begin(), tmp.end(),
                lv_eqn.kill.begin(), lv_eqn.kill.end(),
                std::inserter(lv_ana.entry, lv_ana.entry.begin())
            );

            // Check for fixpoint
            fixpoint &= lv_ana.entry == orig;

            /// exit = U {succs} ///
            orig.clear();
            orig.swap(lv_ana.exit);

            for(auto& succ : block->succs) {
                auto& succ_entry = ana.lv[succ->id].entry;
                tmp.clear();
                std::set_union(
                    lv_ana.exit.begin(), lv_ana.exit.end(),
                    succ_entry.begin(), succ_entry.end(),
                    std::inserter(tmp, tmp.begin())
                );
                lv_ana.entry.swap(tmp);
            }

            // Check for fixpoint
            fixpoint &= lv_ana.exit == orig;
        }
    } while(!fixpoint);

    return ana;
}