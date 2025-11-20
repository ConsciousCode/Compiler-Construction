#pragma once

#include "ast.hpp"

#include <set>

/**
 * Collect all the variables in an AST into a set.
 */
void collect_vars(const ast::Block& b, std::set<std::string>& vars);
