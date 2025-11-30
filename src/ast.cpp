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

std::string ast::to_c(const ast::Cmd& cmd){
    return std::visit([](auto&& c) { return c.to_c(); }, cmd);
}

std::string ast::Block::to_c() const {
    std::string result = "";
    for(const Cmd& cmd : cmds){
        result += ast::to_c(cmd);
        result += "\n";
    }
    return result;
}

std::string ast::If::to_c() const {
    return "if (" + ast::to_c(cond) + "){\n"
        + then.to_c() + "}\n"
        + "else {\n" + el.to_c() + "}\n"; 
}

std::string ast::While::to_c() const {
    return "while (" + ast::to_c(cond) + "){\n"
        + body.to_c() + "}\n";
}
