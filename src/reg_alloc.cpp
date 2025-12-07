#include "reg_alloc.hpp"

#include <vector>
#include <algorithm>

static const std::vector<std::string> REG_POOL = {
    "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11"
};

Allocation allocate_registers(const cfg::CFG& g, const FlowAnalysis& ana) {
    Allocation alloc;

    std::set<std::string> all_vars;
    for (const auto& blk_ptr : g.blocks) {
        auto* b = blk_ptr.get();
        switch (b->kind) {
            case cfg::BlockKind::Assignment: {
                auto* ab = static_cast<cfg::AssignmentBlock*>(b);
                all_vars.insert(ab->lhs);
                all_vars.insert(ab->used_vars.begin(), ab->used_vars.end());
                break;
            }
            case cfg::BlockKind::Condition: {
                auto* cb = static_cast<cfg::ConditionBlock*>(b);
                all_vars.insert(cb->used_vars.begin(), cb->used_vars.end());
                break;
            }
            case cfg::BlockKind::Skip:
                break;
        }
    }

    int slot = 0;
    for (const auto& v : all_vars) {
        alloc.var_to_slot[v] = slot++;
    }

    std::map<std::string, std::set<std::string>> adj;

    for (const auto& [label, info] : ana.lv) {
        const auto& live = info.exit;

        for (auto it1 = live.begin(); it1 != live.end(); it1++) {
            auto it2 = it1;
            it2++;

            for (; it2 != live.end(); it2++) {
                const std::string& x = *it1;
                const std::string& y = *it2;
                if (x == y) {
                    continue;
                }

                adj[x].insert(y);
                adj[y].insert(x);
            }
        }
    }

    for (const auto& v : all_vars) {
        adj[v];
    }

    std::vector<std::string> vars_sorted(all_vars.begin(), all_vars.end());
    std::sort(vars_sorted.begin(), vars_sorted.end(), [&](const std::string& a, const std::string& b) {
        return adj[a].size() > adj[b].size();
    });

    for (const auto& v : vars_sorted) {
        std::set<std::string> forbidden;
        for (const auto& n : adj[v]) {
            auto it = alloc.var_to_reg.find(n);
            if (it != alloc.var_to_reg.end()) {
                forbidden.insert(it->second);
            }
        }

        for (const auto& r : REG_POOL) {
            if (!forbidden.count(r)) {
                alloc.var_to_reg[v] = r;
                break;
            }
        }
    }

    return alloc;
}