#include "cfg.hpp"

#include "ast.hpp"
namespace cfg {

AssignmentBlock::~AssignmentBlock() = default;
ConditionBlock::~ConditionBlock() = default;
SkipBlock::~SkipBlock() = default;

std::string AssignmentBlock::to_c(const ast::CodeGenContext& ctx) const {
    return ctx.indent() + lhs + " = " + ast::to_c(expr, ctx) + ";\n";
}

std::string ConditionBlock::to_c([[maybe_unused]] const ast::CodeGenContext& ctx) const {
    return ""; // Condition is handled in control flow, not as statement
}

std::string SkipBlock::to_c([[maybe_unused]] const ast::CodeGenContext& ctx) const {
    return ""; // Skip does nothing
}

std::string CFG::to_c(const std::string& prog_name, std::set<std::string> vars) const {

    std::string result =
        "#include <stdint.h>\nvoid " + prog_name + "(int64_t vars[]) {\n" + "    int64_t ";

    int index = 0;
    for (const auto& var : vars) {
        if (index > 0)
            result += ", ";
        result += cgen::mangle(var) + " = vars[" + std::to_string(index) + "]";
        ++index;
    }
    result += ";\n";

    ast::CodeGenContext ctx = {1, 0};

    for (const auto& block : blocks) {
        result += "label_" + std::to_string(block->id) + ":\n";
        result += block->to_c(ast::CodeGenContext{1, 0});
        result += generate_control_flow(block.get());
    }

    result += "exit:\n";

    // Store local variables
    index = 0;
    for (const auto& var : vars) {
        result +=
            ctx.indent() + "vars[" + std::to_string(index) + "] = " + cgen::mangle(var) + ";\n";
        ++index;
    }

    result += "}\n";

    return result;
}

std::string CFG::generate_control_flow(CFGBlock* block) const {
    if (block->is_exit) {
        return "    goto exit;\n";
    }

    if (block->kind == BlockKind::Condition) {
        // Condition blocks have 2 successors: true branch, false branch
        if (block->succs.size() == 2) {
            auto* cond = static_cast<ConditionBlock*>(block);
            return "    if (" + ast::to_c(cond->cond, {0, 0}) + ") goto label_" +
                   std::to_string(block->succs[0]->id) + ";\n" + "    else goto label_" +
                   std::to_string(block->succs[1]->id) + ";\n";
        }
    }

    // For assignment/skip, just goto next block
    if (!block->succs.empty()) {
        return "    goto label_" + std::to_string(block->succs[0]->id) + ";\n";
    }

    return "";
}

} // namespace cfg
