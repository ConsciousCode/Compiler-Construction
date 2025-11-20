#pragma once

#include "ast.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cfg {

    enum class BlockKind {
        Assignment,
    };

    struct CFGBlock {
        std::size_t id{};

        BlockKind kind;
        std::vector<CFGBlock*> preds;
        std::vector<CFGBlock*> succs;

        bool is_entry{false};
        bool is_exit{false};

        virtual ~CFGBlock() = default;
    };

    struct AssignmentBlock : public CFGBlock {
        std::string lhs;
        ast::Expr expr;
        std::vector<std::string> used_vars;
    };

    struct CFG {
        std::vector<std::unique_ptr<CFGBlock>> blocks;

        CFGBlock* entry{nullptr};
        CFGBlock* exit{nullptr};

        void add_edge(CFGBlock* from, CFGBlock* to) {
            if (!from || !to) return;
            from->succs.push_back(to);
            to->preds.push_back(from);
        }
    };
}