#pragma once
#include "parser.hpp"
#include "ast.hpp"
#include "whilelang.hpp"

ast::Block build_ast_from_parse(const ParseTreeNode& root);