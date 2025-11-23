/**
 * Generate While code from the AST.
 */

#include "ast.hpp"

std::string to_while(const ast::Arith& arith);
std::string to_while(const ast::Cond& cond);
std::string to_while(const ast::Assign& assign);
std::string to_while(const ast::Block& block);
std::string to_while(const ast::Expr& expr);