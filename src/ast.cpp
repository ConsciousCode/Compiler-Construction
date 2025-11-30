#include "ast.hpp"

std::string expr_to_c(const ast::Expr& expr) {
        return std::visit([](auto&& e) -> std::string {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, int>)
                return std::to_string(e);
            else if constexpr (std::is_same_v<T, std::string>)
                return e;
            else if constexpr (std::is_same_v<T, std::shared_ptr<ast::Arith>>)
                return e->to_c();  // Recursively call to_c on the pointed-to Arith
            else
                static_assert(false, "non-exhaustive visitor!");
        }, expr);
}


