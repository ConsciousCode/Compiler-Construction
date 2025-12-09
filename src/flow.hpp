#pragma once

#include <functional>
#include <set>
#include <map>

#include "cfg.hpp"

/**
 * (var, label) pair used in flow analyses. (Currently unused)
**/
struct AnaTerm {
    std::string var;
    size_t label;

    inline bool operator==(const AnaTerm& other) const {
        return var == other.var && label == other.label;
    }

    inline bool operator<(const AnaTerm& other) const {
        return var < other.var || label < other.label;
    }
};

namespace std {
template <>
struct hash<AnaTerm> {
    inline size_t operator()(const AnaTerm term) const {
        return hash<std::string>{}(term.var)*0x9e3779b97f4a7c15 + hash<size_t>{}(term.label);
    }
};
}  // namespace std

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

/**
 * Combined entry/exit information for an analysis
**/
template<typename T>
struct Analysis {
    std::set<T> entry;
    std::set<T> exit;
    FlowEq eqn;
};

/**
 * The results of flow analysis over a CFG.
**/
struct FlowAnalysis {
    std::map<size_t, Analysis<std::string>> lv; // Live Variable analysis
    std::set<std::string> exit_live;

    void print_equations(const cfg::CFG& cfg) const;
    void print_solution() const;
};

FlowAnalysis flow_analysis(const cfg::CFG& cfg, const std::set<std::string>& exit_live);
