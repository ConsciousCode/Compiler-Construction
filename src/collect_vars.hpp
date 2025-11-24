#pragma once

#include "ast.hpp"

#include <set>

/**
 * Collect all the variables in an AST into a set.
 */
void collect_vars(const ast::Expr& e, std::set<std::string>& vars);
void collect_vars(const ast::Cond& c, std::set<std::string>& vars);
void collect_vars(const ast::Cmd& c, std::set<std::string>& vars);
void collect_vars(const ast::Block& b, std::set<std::string>& vars);
