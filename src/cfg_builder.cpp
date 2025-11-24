#include "cfg_builder.hpp"

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace {

struct Fragment {
    cfg::CFGBlock* entry = nullptr;
    std::vector<cfg::CFGBlock*> exits;
};

void collect_used_vars(const ast::Expr& e, std::vector<std::string>& out);
void collect_used_vars_cond(const ast::Cond& c, std::vector<std::string>& out);
Fragment build_block(const ast::Block& block, cfg::CFG& g);
Fragment build_cmd(const ast::Cmd& cmd, cfg::CFG& g);
Fragment build_assign(const ast::Assign& asg, cfg::CFG& g);
Fragment build_if(const ast::If& iff, cfg::CFG& g);
Fragment build_while(const ast::While& wh, cfg::CFG& g);
Fragment build_skip(cfg::CFG& g);

void collect_used_vars(const ast::Expr& e, std::vector<std::string>& out) {
    std::visit(
        [&](auto const& v) {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, std::string>) {
                out.push_back(v);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                collect_used_vars(v->left, out);
                collect_used_vars(v->right, out);
            } else if constexpr (std::is_same_v<T, int>) {
            } else {
                static_assert(sizeof(T) == 0, "Unhandled Expr alternative");
            }
        },
        e);
}

void collect_used_vars_cond(const ast::Cond& c, std::vector<std::string>& out) {
    std::visit(
        [&](auto const& ptr) {
            using PtrT = std::decay_t<decltype(ptr)>;

            if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::BoolConst>>) {
                (void)ptr;
            } else if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::Not>>) {
                collect_used_vars_cond(ptr->cond, out);
            } else if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::Logical>>) {
                collect_used_vars_cond(ptr->left, out);
                collect_used_vars_cond(ptr->right, out);
            } else if constexpr (std::is_same_v<PtrT, std::shared_ptr<ast::Comparison>>) {
                collect_used_vars(ptr->left, out);
                collect_used_vars(ptr->right, out);
            } else {
                static_assert(sizeof(PtrT) == 0, "Unhandled Cond alternative");
            }
        },
        c);
}

Fragment build_block(const ast::Block& block, cfg::CFG& g) {
    if (block.cmds.empty()) {
        throw std::runtime_error("build_block: emtpty blocks not supported yet");
    }

    Fragment current;
    bool first = true;

    for (auto const& cmd : block.cmds) {
        Fragment f = build_cmd(cmd, g);

        if (first) {
            current = f;
            first = false;
        } else {
            for (cfg::CFGBlock* exit : current.exits) {
                g.add_edge(exit, f.entry);
            }
            current.exits = f.exits;
        }
    }

    return current;
}

Fragment build_cmd(const ast::Cmd& cmd, cfg::CFG& g) {
    return std::visit(
        [&](auto const& concrete) -> Fragment {
            using T = std::decay_t<decltype(concrete)>;

            if constexpr (std::is_same_v<T, ast::Assign>) {
                return build_assign(concrete, g);
            } else if constexpr (std::is_same_v<T, ast::If>) {
                return build_if(concrete, g);
            } else if constexpr (std::is_same_v<T, ast::While>) {
                return build_while(concrete, g);
            } else if constexpr (std::is_same_v<T, ast::Skip>) {
                return build_skip(g);
            } else {
                static_assert(sizeof(T) == 0, "Unhandled Cmd varient");
            }
        },
        cmd);
}

Fragment build_assign(const ast::Assign& asg, cfg::CFG& g) {
    auto block = std::make_unique<cfg::AssignmentBlock>();
    auto* raw = block.get();

    raw->id = g.blocks.size();
    raw->lhs = asg.name;
    raw->expr = asg.value;

    collect_used_vars(raw->expr, raw->used_vars);
    g.blocks.push_back(std::move(block));

    Fragment f;
    f.entry = raw;
    f.exits = {raw};
    return f;
}

Fragment build_if(const ast::If& iff, cfg::CFG& g) {
    auto cond_block = std::make_unique<cfg::ConditionBlock>();
    auto* c = cond_block.get();

    c->id = g.blocks.size();
    c->cond = iff.cond;

    collect_used_vars_cond(c->cond, c->used_vars);

    g.blocks.push_back(std::move(cond_block));

    Fragment then_frag = build_block(iff.then, g);
    Fragment else_frag = build_block(iff.el, g);

    g.add_edge(c, then_frag.entry);
    g.add_edge(c, else_frag.entry);

    Fragment result;
    result.entry = c;
    result.exits = then_frag.exits;
    result.exits.insert(result.exits.end(), else_frag.exits.begin(), else_frag.exits.end());

    return result;
}

Fragment build_while(const ast::While& wh, cfg::CFG& g) {
    auto cond_block = std::make_unique<cfg::ConditionBlock>();
    auto* c = cond_block.get();

    c->id = g.blocks.size();
    c->cond = wh.cond;

    collect_used_vars_cond(c->cond, c->used_vars);

    g.blocks.push_back(std::move(cond_block));

    Fragment body_frag = build_block(wh.body, g);

    g.add_edge(c, body_frag.entry);

    for (cfg::CFGBlock* exit : body_frag.exits) {
        g.add_edge(exit, c);
    }

    Fragment result;
    result.entry = c;
    result.exits = {c};

    return result;
}

Fragment build_skip(cfg::CFG& g) {
    auto block = std::make_unique<cfg::SkipBlock>();
    auto* raw = block.get();

    raw->id = g.blocks.size();
    g.blocks.push_back(std::move(block));

    Fragment f;
    f.entry = raw;
    f.exits = {raw};
    return f;
}
} // namespace

namespace cfg {

CFG build_cfg(const ast::Block& program) {
    CFG g;

    Fragment prog_frag = build_block(program, g);

    if (prog_frag.entry) {
        g.entry = prog_frag.entry;
        g.entry->is_entry = true;
    }

    if (!prog_frag.exits.empty()) {
        g.exit = prog_frag.exits.front();
        g.exit->is_exit = true;
    }

    return g;
}
} // namespace cfg