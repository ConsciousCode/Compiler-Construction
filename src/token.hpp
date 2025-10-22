#pragma once

#include <cstdint>
#include <iosfwd>

/**
 * Stream location metadata.
 */
struct Loc {
    int line, col, pos;

    bool operator==(const Loc& other) const {
        return line == other.line && col == other.col && pos == other.pos;
    }

    bool operator!=(const Loc& other) const {
        return !(*this == other);
    }
};

std::ostream& operator<<(std::ostream& os, const Loc& loc);

/**
 * Type of a token.
 */
enum class TokenType {
    ERROR = 0,
    ERROR_INT = 1,     // Integer followed by alpha
    ERROR_EOF = 2,     // End of file
    ERROR_CHAR = 3,    // Unknown character
    ERROR_COLON = 4,   // Bare colon without an equals sign
    ERROR_COMMENT = 5, // Unclosed multiline comment

    IDENT = 6,
    INTEGER = 7,

    KEYWORD = 8,
    KW_TRUE = 9,
    KW_FALSE = 10,
    KW_IF = 11,
    KW_THEN = 12,
    KW_ELSE = 13,
    KW_FI = 14,
    KW_WHILE = 15,
    KW_DO = 16,
    KW_OD = 17,
    KW_SKIP = 18,

    ARITH_OP = 19,
    OP_ADD = 20,
    OP_SUB = 21,
    OP_MUL = 22,

    BOOL_OP = 23,
    OP_NOT = 24,
    OP_AND = 25,
    OP_OR = 26,

    REL_OP = 27,
    OP_EQ = 28,
    OP_LT = 29,
    OP_GT = 30,
    OP_LE = 31,
    OP_GE = 32,

    PUNC_OP = 33,
    OP_ASSIGN = 34,
    OP_SEMI = 35,
    OP_LPAREN = 36,
    OP_RPAREN = 37,
    OP_LBRACK = 38,
    OP_RBRACK = 39,

    COUNT = 40
};

std::ostream& operator<<(std::ostream& os, const TokenType& tt);

/**
 * Language token.
 */
struct Token {
    Loc loc;
    int length;
    TokenType type;
    int64_t value;

    Token(Loc loc, int length, TokenType type, int64_t value = 0);

    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;

    bool is_error() const;
};

std::ostream& operator<<(std::ostream& os, const Token& t);
