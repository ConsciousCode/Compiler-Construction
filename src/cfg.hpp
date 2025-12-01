#pragma once

#include "ast.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace cfg {

enum class BlockKind { Assignment, Condition, Skip };

struct CFGBlock {
    std::size_t id{};

    BlockKind kind;
    std::vector<CFGBlock*> preds;
    std::vector<CFGBlock*> succs;

    bool is_entry{false};
    bool is_exit{false};

    virtual std::string to_c(const ast::CodeGenContext& ctx) const = 0;
    virtual ~CFGBlock() = default;
};

struct AssignmentBlock : public CFGBlock {
    std::string lhs;
    ast::Expr expr;
    std::vector<std::string> used_vars;

    AssignmentBlock() {
        kind = BlockKind::Assignment;
    }
    ~AssignmentBlock();

    std::string to_c(const ast::CodeGenContext& ctx) const override;
};

struct ConditionBlock : public CFGBlock {
    ast::Cond cond;
    std::vector<std::string> used_vars;

    ConditionBlock() {
        kind = BlockKind::Condition;
    }
    ~ConditionBlock();

    std::string to_c(const ast::CodeGenContext& ctx) const override;
};

struct SkipBlock : public CFGBlock {
    SkipBlock() {
        kind = BlockKind::Skip;
    }
    ~SkipBlock();

    std::string to_c(const ast::CodeGenContext& ctx) const override;
};

struct CFG {
    std::vector<std::unique_ptr<CFGBlock>> blocks;

    CFGBlock* entry{nullptr};
    CFGBlock* exit{nullptr};

    void add_edge(CFGBlock* from, CFGBlock* to) {
        if (!from || !to)
            return;
        from->succs.push_back(to);
        to->preds.push_back(from);
    }

    std::string generate_control_flow(CFGBlock* block) const;
    std::string to_c(const std::string& prog_name, std::set<std::string> vars) const;
};
} // namespace cfg
