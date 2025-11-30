#include "ast.hpp"

std::string to_c(const ast::Expr& expr) {
        return std::visit([](auto&& e) -> std::string {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, int>)
                return std::to_string(e);
            else if constexpr (std::is_same_v<T, std::string>)
                return e;
            else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>)
                return e->to_c();
            else{
                throw std::runtime_error("Attempted to convert unkown Arithmetic Expression to c.");
            }
        }, expr);
}

std::string to_c(const ast::Cond& cond) {
    return std::visit([](auto&& c) { return c->to_c(); }, cond);
}
