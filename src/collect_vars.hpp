#pragma once

#include <set>
#include "ast.hpp"

/**
 * Collect all the variables in an AST into a set.
 */
void collect_vars(const ast::Block& b, std::set<std::string>& vars);
