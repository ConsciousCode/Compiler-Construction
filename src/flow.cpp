#include "flow.hpp"
#include "cfg.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>

#include <iostream>

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
std::ostream& operator<<(std::ostream& os, const std::set<T>& s) {
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

void FlowAnalysis::print_equations(const cfg::CFG& cfg) const {
    for(const auto& [label, ana] : lv) {
        std::cout << "LA◦(" << label << ") = LA•(" << label << ")";
        if(ana.eqn.kill.size()) {
            std::cout << " \\ " << ana.eqn.kill;
        }
        if(ana.eqn.gen.size()) {
            std::cout << " ∪ " << ana.eqn.gen;
        }
        std::cout << std::endl;

        std::cout << "LA•(" << label << ") = ";
        const auto& block = *cfg.blocks[label];
        if(block.succs.size() == 0) {
            std::cout << exit_live;
        }
        else {
            auto it = block.succs.begin();
            std::cout << "LA◦(" << (*it++)->id << ")";
            while(it != block.succs.end()) {
                std::cout << " ∪ LA◦(" << (*it++)->id << ")";
            }
        }
        std::cout << std::endl;
    }
}

void FlowAnalysis::print_solution() const {
    for(const auto& [label, ana] : lv) {
        std::cout << "LA◦(" << label << ") = " << ana.entry << std::endl;
        std::cout << "LA•(" << label << ") = " << ana.exit << std::endl;
    }
}

struct _Analyze {
    FlowAnalysis ana;
    
    _Analyze(const cfg::CFG& cfg, const std::set<std::string> el) {
        ana.exit_live = el;
        for(const auto& block : cfg.blocks) {
            FlowEq lv_eqn;
            lv_genset(*block, lv_eqn.gen);
            lv_killset(*block, lv_eqn.kill);
            ana.lv[block->id].eqn = lv_eqn;
        }
    }

    bool lv_fixedpoint(const cfg::CFGBlock& block) {
        bool fixed = true;
        auto& eqn = ana.lv[block.id].eqn;
        auto& lv = ana.lv[block.id];

        /// entry = exit \ kill ∪ gen ///
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

        // entry = tmp ∪ gen
        lv.entry.insert(eqn.gen.begin(), eqn.gen.end());

        // Check for fixed point
        fixed &= lv.entry == orig;

        /// exit = ∪ {succs} ///
        orig.clear();
        orig.swap(lv.exit);
        assert(lv.exit.size() == 0);

        if(block.succs.size() == 0) {
            lv.exit.insert(ana.exit_live.begin(), ana.exit_live.end());
        }
        else {
            for(auto& succ : block.succs) {
                auto& lvo = ana.lv[succ->id].entry;
                lv.exit.insert(lvo.begin(), lvo.end());
            }
        }

        // Check for fixed point
        fixed &= lv.exit == orig;

        return fixed;
    }

    FlowAnalysis analyze(const cfg::CFG& cfg) {
        int limit = 0;
        bool fixed;
        do {
            // true -> false whenever any set is changed
            fixed = true;
            for(const auto& block : cfg.blocks) {
                fixed &= lv_fixedpoint(*block);
            }
        } while(!fixed && ++limit < 30);

        return ana;
    }
};

FlowAnalysis flow_analysis(const cfg::CFG& cfg, const std::set<std::string>& exit_live) {
    _Analyze flow{cfg, exit_live};
    return flow.analyze(cfg);
}

std::set<std::string> live_vars_at_entry(const cfg::CFG &g) {
    FlowAnalysis ana = flow_analysis(g, {"output"});

    auto entry_id = g.entry->id;
    auto it = ana.lv.find(entry_id);
    if (it == ana.lv.end()) {
        throw std::runtime_error("No liveness infor for entry block");
    }
    return it->second.entry;
}
