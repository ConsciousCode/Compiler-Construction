#pragma once

#include <functional>
#include <unordered_set>
#include <unordered_map>

#include "cfg.hpp"

/**
 * (var, label) pair used in flow analyses.
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
 * Combined entry/exit information for an analysis
**/
struct EntryExit {
    std::unordered_set<AnaTerm> entry;
    std::unordered_set<AnaTerm> exit;
};

/**
 * The results of flow analysis over a CFG.
**/
struct Analysis {
    std::unordered_map<size_t, EntryExit> lv; // Live Variable analysis
};

Analysis flow_analysis(const cfg::CFG& cfg);
