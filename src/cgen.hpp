/**
 * Generate C code from the AST.
 */
#pragma once

#include <string>

// Forward declarations
namespace ast {
class Block;
}

namespace cgen {
/**
 * Generate C code from a While language AST.
 *
 * @param entry The name of the generated C function
 * @param block The root block of the AST
 * @return A string containing the generated C code
 */
std::string to_c(std::string entry, const ast::Block& block);

/**
 * Replace all occurrences of a substring in a string.
 */
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);

/**
 * Indent a string by adding 4 spaces to each line.
 */
std::string indent(std::string str);

/**
 * Mangle a variable name for C output.
 * Replaces underscores with double underscores and apostrophes with underscores.
 */
std::string mangle(std::string var);
} // namespace cgen
