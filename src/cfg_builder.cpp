#include "cfg_builder.hpp"

#include <stdexcept>
#include <variant>

namespace {
    void collect_used_vars(const ast::Expr& e, std::vector<std::string>& out) {
        std::visit(
            [&](auto const& v) {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, std::string>) {
                    out.push_back(v);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>) {
                    if (!v) return;

                    collect_used_vars(v->left, out);
                    collect_used_vars(v->right, out);
                } else if constexpr (std::is_same_v<T, int>){
                } else {
                    static_assert(sizeof(T) == 0, "Unhandled Expr alternative");
                }
            },
            e
        );
    }
}

namespace cfg {
    CFG build_cfg(const ast::Block& program) {
        CFG g;

        CFGBlock* prev = nullptr;

        for (const auto& cmd : program.cmds) {
            const ast::Assign* asg = std::get_if<ast::Assign>(&cmd);
            if (!asg) {
                throw std::runtime_error("build_cfg: only Assign commands are support in this version.");
            }

            auto block = std::make_unique<AssignmentBlock>();
            auto* raw = block.get();

            raw->id = g.blocks.size();
            raw->lhs = asg->name;
            raw->expr = asg->value;

            collect_used_vars(raw->expr, raw->used_vars);

            if (prev) {
                g.add_edge(prev, raw);
            }

            g.blocks.push_back(std::move(block));
            prev = raw;
        }

        if (!g.blocks.empty()) {
            g.entry = g.blocks.front().get();
            g.exit = g.blocks.back().get();
            g.entry->is_entry = true;
            g.exit->is_exit = true;
        }

        return g;
    }
}