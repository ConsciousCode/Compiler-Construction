#pragma once

#include "cfg.hpp"
#include "flow.hpp"

#include <map>
#include <set>
#include <string>

struct Allocation {
    std::map<std::string, std::string> var_to_reg;

    std::map<std::string, int> var_to_slot;
};

Allocation allocate_registers(const cfg::CFG& g, const FlowAnalysis& ana);