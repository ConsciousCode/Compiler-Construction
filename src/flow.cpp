#include "flow.hpp"
#include "cfg.hpp"
#include <algorithm>
#include <iterator>

#include <iostream>

/**
 * Flow analysis equation, its use depends on the type of analysis being
 * done. All of them are of the form V(l) = V'(l) \ kill U gen.
 * Analyses differ in which V and V' are (entry vs exit) and if they're
 * combined with union or intersection.
**/
struct AnaEq {
    std::unordered_set<AnaTerm> gen;
    std::unordered_set<std::string> kill;
};

void lv_genset(cfg::CFGBlock& block, std::unordered_set<AnaTerm>& gen) {
    switch(block.kind) {
        case cfg::BlockKind::Assignment: {
            auto *ablock = (cfg::AssignmentBlock*)&block;
            for(const auto& fv : ablock->used_vars) {
                gen.insert(AnaTerm{fv, block.id});
            }
            break;
        }
        
        case cfg::BlockKind::Condition: {
            auto *cblock = (cfg::ConditionBlock*)&block;
            for(const auto& fv : cblock->used_vars) {
                gen.insert(AnaTerm{fv, block.id});
            }
            break;
        }
        case cfg::BlockKind::Skip:
            break;
    }
}

void lv_killset(cfg::CFGBlock& block, std::unordered_set<std::string>& kill) {
    switch(block.kind) {
        case cfg::BlockKind::Assignment: {
            auto *ablock = (cfg::AssignmentBlock*)&block;
            kill.insert(ablock->lhs);
            break;
        }
        
        case cfg::BlockKind::Condition:
        case cfg::BlockKind::Skip:
            break;
    }
}

std::ostream& operator<<(std::ostream& os, const AnaTerm& ana) {
    return os << "(" << ana.var << ", " << ana.label << ")";
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::unordered_set<T>& s) {
    os << "{";
    if(s.size() > 0) {
        auto it = s.begin();
        os << *it++;
        while(it != s.end()) {
            os << ", " << *it++;
        }
    }
    return os << "}";
}

Analysis flow_analysis(const cfg::CFG& cfg) {
    // Build equations
    std::unordered_map<size_t, AnaEq> lv_eqns;
    std::unordered_set<AnaTerm> orig;
    std::unordered_set<AnaTerm> tmp;

    for(const auto& block : cfg.blocks) {
        AnaEq lv_eqn;
        lv_genset(*block, lv_eqn.gen);
        lv_killset(*block, lv_eqn.kill);
        lv_eqns[block->id] = lv_eqn;

        std::cout << block->id << ": gen " << lv_eqn.gen << " | kill " << lv_eqn.kill << std::endl;
    }

    // Fixpoint equation solving
    Analysis ana; // Initialize to empty
    int limit = 0;
    bool fixpoint;
    do {
        // true -> false whenever any set is changed
        fixpoint = true;
        for(const auto& block : cfg.blocks) {
            auto& lv_eqn = lv_eqns[block->id];
            auto& lv_ana = ana.lv[block->id];

            /// entry = exit \ kill U gen ///
            orig.clear();
            tmp.clear();

            orig.swap(lv_ana.entry);
            
            // tmp = exit \ kill
            for(const auto& ana : lv_ana.exit) {
                if(!lv_eqn.kill.count(ana.var)) {
                    tmp.insert(ana);
                }
            }

            // entry = tmp U gen
            std::set_union(
                tmp.begin(), tmp.end(),
                lv_eqn.gen.begin(), lv_eqn.gen.end(),
                std::inserter(lv_ana.entry, lv_ana.entry.begin())
            );

            // Check for fixpoint
            fixpoint &= lv_ana.entry == orig;
            if(lv_ana.entry != orig) {
                std::cout << "Fixpoint failed entry: " << block->id << " " << lv_ana.entry << " | " << orig << std::endl;
            }

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
                lv_ana.exit.swap(tmp);
            }

            // Check for fixpoint
            fixpoint &= lv_ana.exit == orig;
            if(lv_ana.exit != orig) {
                std::cout << "Fixpoint failed exit: " << block->id << " " << lv_ana.exit << " | " << orig << std::endl;
            }
        }
    } while(!fixpoint && ++limit < 20);

    std::cout << "Limit " << limit << std::endl;

    for(auto& [label, ox] : ana.lv) {
        std::cout << label << ": entry " << ox.entry << " | exit " << ox.exit << std::endl;
    }

    return ana;
}
