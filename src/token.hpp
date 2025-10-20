#pragma once

#include <cstdint>
#include <iosfwd>

/**
 * Stream location metadata.
 */
struct Loc {
    int line, col, pos;

    bool operator==(const Loc& other) const {
        return line == other.line
            && col == other.col
            && pos == other.pos;
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
    TOK_EOF = 1,
    IDENT = 0x10,
    INTEGER = 0x11,

    KEYWORD = 0x20,
    KW_TRUE = KEYWORD | 0,
    KW_FALSE = KEYWORD | 1,
    KW_IF = KEYWORD | 2,
    KW_THEN = KEYWORD | 3,
    KW_ELSE = KEYWORD | 4,
    KW_FI = KEYWORD | 5,
    KW_WHILE = KEYWORD | 6,
    KW_DO = KEYWORD | 7,
    KW_OD = KEYWORD | 8,
    KW_SKIP = KEYWORD | 9,

    ARITH_OP = 0x30,
    OP_ADD = ARITH_OP | 0,
    OP_SUB = ARITH_OP | 1,
    OP_MUL = ARITH_OP | 2,

    BOOL_OP = 0x40,
    OP_NOT = BOOL_OP | 0,
    OP_AND = BOOL_OP | 1,
    OP_OR = BOOL_OP | 2,

    REL_OP = 0x50,
    OP_EQ = REL_OP | 0,
    OP_LT = REL_OP | 1,
    OP_GT = REL_OP | 2,
    OP_LE = REL_OP | 3,
    OP_GE = REL_OP | 4,

    PUNC_OP = 0x60,
    OP_ASSIGN = PUNC_OP | 0,
    OP_SEMI = PUNC_OP | 1,
    OP_LPAREN = PUNC_OP | 2,
    OP_RPAREN = PUNC_OP | 3,
    OP_LBRACK = PUNC_OP | 4,
    OP_RBRACK = PUNC_OP | 5
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

    Token(Loc loc, int length, TokenType type, int64_t value = 0)
        : loc(loc), length(length), type(type), value(value) {}

    bool operator==(const Token& other) const {
        return loc == other.loc 
            && length == other.length
            && type == other.type
            && value == other.value;
    }

    bool operator!=(const Token& other) const {
        return !(*this == other);
    }

};

std::ostream& operator<<(std::ostream& os, const Token& t);
