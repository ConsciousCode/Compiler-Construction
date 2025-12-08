#include "ast.hpp"

#include "cgen.hpp"
#include "collect_vars.hpp"

#include <string>

namespace ast {
std::string to_c(const ast::Expr& expr, const CodeGenContext& ctx) {
    return std::visit(
        [&ctx](auto&& e) -> std::string {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, int>)
                return std::to_string(e);
            else if constexpr (std::is_same_v<T, std::string>)
                return cgen::mangle(e);
            else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>)
                return e->to_c(ctx);
            else {
                throw std::runtime_error("Attempted to convert unkown Arithmetic Expression to c.");
            }
        },
        expr);
}

std::string ast::Arith::to_c(const CodeGenContext& ctx) const {
    std::string code =
        ast::to_c(left, ctx.inc_depth()) + to_c(op) + ast::to_c(right, ctx.inc_depth());

    return ctx.parenthesize(code);
}

std::string ast::BoolConst::to_c([[maybe_unused]] const CodeGenContext& ctx) const {
    std::string str_value = value ? "1/*true*/" : "0/*false*/";
    return str_value;
}

std::string ast::Not::to_c(const CodeGenContext& ctx) const {
    return ctx.parenthesize("!" + ast::to_c(cond, ctx.inc_depth()));
}

std::string ast::Logical::to_c(const CodeGenContext& ctx) const {
    std::string result =
        ast::to_c(left, ctx.inc_depth()) + to_c(op) + ast::to_c(right, ctx.inc_depth());

    return ctx.parenthesize(result);
}

std::string ast::Comparison::to_c(const CodeGenContext& ctx) const {
    std::string result =
        ast::to_c(left, ctx.inc_depth()) + to_c(op) + ast::to_c(right, ctx.inc_depth());

    return ctx.parenthesize(result);
}

std::string to_c(const ast::Cond& cond, const CodeGenContext& ctx) {
    return std::visit([&ctx](auto&& c) { return c->to_c(ctx); }, cond);
}

std::string to_c(const ast::Cmd& cmd, const CodeGenContext& ctx) {
    return std::visit([&ctx](auto&& c) { return c.to_c(ctx); }, cmd);
}

std::string ast::Skip::to_c([[maybe_unused]] const CodeGenContext& ctx) const {
    return ";";
}

std::string ast::Assign::to_c(const CodeGenContext& ctx) const {
    std::string result = cgen::mangle(name) + " = " + ast::to_c(value, ctx) + ";";
    return ctx.indent() + result;
}

std::string ast::Block::to_c(const CodeGenContext& ctx) const {
    std::string result = "";
    for (const Cmd& cmd : cmds) {
        result += ast::to_c(cmd, ctx);
        result += "\n";
    }
    return result;
}

std::string ast::If::to_c(const CodeGenContext& ctx) const {
    std::string result = ctx.indent() + "if (" + ast::to_c(cond, ctx.depth_reset()) + ") {\n" +
                         then.to_c(ctx.inc_indent()) + ctx.indent() + "}\n" + ctx.indent() +
                         "else {\n" + el.to_c(ctx.inc_indent()) + ctx.indent() + "}\n";
    return result;
}

std::string ast::While::to_c(const CodeGenContext& ctx) const {
    std::string result = ctx.indent() + "while (" + ast::to_c(cond, ctx.depth_reset()) + ") {\n" +
                         body.to_c(ctx.inc_indent()) + ctx.indent() + "}\n";
    return result;
}

std::string ast::AST::to_c(const std::string& prog_name) const {
    std::set<std::string> vars;
    collect_vars(*root, vars);

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

    CodeGenContext ctx = {1, 0};

    for (const Cmd& cmd : root->cmds) {
        result += ast::to_c(cmd, ctx);
        result += "\n";
    }

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

ast::Skip::~Skip() = default;

} // namespace ast
