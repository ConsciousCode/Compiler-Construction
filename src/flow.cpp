#include "flow.hpp"
#include "cfg.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>

#include <iostream>

/**
 * Flow analysis equation, its use depends on the type of analysis being
 * done. All of them are of the form V(l) = V'(l) \ kill U gen.
 * Analyses differ in which V and V' are (entry vs exit) and if they're
 * combined with union or intersection.
**/
struct FlowEq {
    std::set<std::string> gen;
    std::set<std::string> kill;
};

struct Equations {
    std::unordered_map<size_t, FlowEq> lv;
};

void lv_genset(cfg::CFGBlock& block, std::set<std::string>& gen) {
    switch(block.kind) {
        case cfg::BlockKind::Assignment: {
            auto *ablock = (cfg::AssignmentBlock*)&block;
            auto& used = ablock->used_vars;
            gen.insert(used.begin(), used.end());
            break;
        }
        
        case cfg::BlockKind::Condition: {
            auto *cblock = (cfg::ConditionBlock*)&block;
            auto& used = cblock->used_vars;
            gen.insert(used.begin(), used.end());
            break;
        }
        case cfg::BlockKind::Skip:
            break;
    }
}

void lv_killset(cfg::CFGBlock& block, std::set<std::string>& kill) {
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
std::ostream& operator<<(std::ostream& os, std::set<T>& s) {
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

struct FlowAnalysis {
    Analysis ana;
    Equations eqns;
    
    FlowAnalysis(const cfg::CFG& cfg) {
        for(const auto& block : cfg.blocks) {
            FlowEq lv_eqn;
            lv_genset(*block, lv_eqn.gen);
            lv_killset(*block, lv_eqn.kill);
            eqns.lv[block->id] = lv_eqn;

            std::cout << block->id << ": gen " << lv_eqn.gen << " | kill " << lv_eqn.kill << std::endl;
        }
    }

    bool lv_fixedpoint(const cfg::CFGBlock& block) {
        bool fixed = true;
        auto& eqn = eqns.lv[block.id];
        auto& lv = ana.lv[block.id];

        /// entry = exit \ kill U gen ///
        std::set<std::string> orig;
        std::set<std::string> tmp;

        orig.swap(lv.entry);
        assert(lv.entry.size() == 0);
        
        // tmp = exit \ kill
        std::set_difference(
            lv.exit.begin(), lv.exit.end(),
            eqn.kill.begin(), eqn.kill.end(),
            std::inserter(lv.entry, lv.entry.begin())
        );

        // entry = tmp U gen
        lv.entry.insert(eqn.gen.begin(), eqn.gen.end());

        // Check for fixed point
        fixed &= lv.entry == orig;
        if(lv.entry != orig) {
            std::cout << "Fixedpoint entry: " << block.id << " " << orig << " != " << lv.entry << " = " << lv.exit << " \\ " << eqn.kill << " U " << eqn.gen << std::endl;
        }

        /// exit = U {succs} ///
        orig.clear();
        orig.swap(lv.exit);
        assert(lv.exit.size() == 0);

        for(auto& succ : block.succs) {
            auto& lvo = ana.lv[succ->id].entry;
            lv.exit.insert(lvo.begin(), lvo.end());
        }

        // Check for fixed point
        fixed &= lv.exit == orig;
        if(lv.exit != orig) {
            std::cout << "Fixedpoint exit: " << block.id << " " << lv.exit << " | " << orig << std::endl;
        }

        return fixed;
    }

    Analysis analyze(const cfg::CFG& cfg) {
        int limit = 0;
        bool fixed;
        do {
            // true -> false whenever any set is changed
            fixed = true;
            for(const auto& block : cfg.blocks) {
                fixed &= lv_fixedpoint(*block);
            }
            std::cout << "loop" << std::endl;
        } while(!fixed && ++limit < 30);

        std::cout << "Limit " << limit << std::endl;

        for(auto& [label, ox] : ana.lv) {
            std::cout << label << ": entry " << ox.entry << " | exit " << ox.exit << std::endl;
        }
        return ana;
    }
};

Analysis flow_analysis(const cfg::CFG& cfg) {
    FlowAnalysis flow{cfg};
    return flow.analyze(cfg);
}
