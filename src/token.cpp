#include <ostream>

#include "token.hpp"

std::ostream& operator<<(std::ostream& os, const Loc& loc) {
    return os << "Loc{"
        << loc.line << ","
        << loc.col << ","
        << loc.pos << "}";
}

const char *to_string(TokenType tt) {
    switch(tt) {
        case TokenType::ERROR: return "ERROR";
        case TokenType::ERROR_INT: return "ERROR_INT";
        case TokenType::ERROR_EOF: return "EOF";
        case TokenType::ERROR_CHAR: return "ERROR_CHAR";
        case TokenType::ERROR_COLON: return "ERROR_COLON";
        case TokenType::ERROR_COMMENT: return "ERROR_COMMENT";
        case TokenType::IDENT: return "IDENT";
        case TokenType::INTEGER: return "INTEGER";

        case TokenType::KW_TRUE: return "KW_TRUE";
        case TokenType::KW_FALSE: return "KW_FALSE";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_THEN: return "KW_THEN";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_FI: return "KW_FI";
        case TokenType::KW_WHILE: return "KW_WHILE";
        case TokenType::KW_DO: return "KW_DO";
        case TokenType::KW_OD: return "KW_OD";
        case TokenType::KW_SKIP: return "KW_SKIP";

        case TokenType::OP_ADD: return "OP_ADD";
        case TokenType::OP_SUB: return "OP_SUB";
        case TokenType::OP_MUL: return "OP_MUL";

        case TokenType::OP_NOT: return "OP_NOT";
        case TokenType::OP_AND: return "OP_AND";
        case TokenType::OP_OR: return "OP_OR";

        case TokenType::OP_EQ: return "OP_EQ";
        case TokenType::OP_LT: return "OP_LT";
        case TokenType::OP_GT: return "OP_GT";
        case TokenType::OP_LE: return "OP_LE";
        case TokenType::OP_GE: return "OP_GE";

        case TokenType::OP_ASSIGN: return "OP_ASSIGN";
        case TokenType::OP_SEMI: return "OP_SEMI";
        case TokenType::OP_LPAREN: return "OP_LPAREN";
        case TokenType::OP_RPAREN: return "OP_RPAREN";
        case TokenType::OP_LBRACK: return "OP_LBRACK";
        case TokenType::OP_RBRACK: return "OP_RBRACK";

        default: return "<unknown>";
    }
}

std::ostream& operator<<(std::ostream& os, const TokenType& tt) {
    return os << "TokenType::" << to_string(tt);
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
    return os << "Token{"
        << t.loc << ", "
        << t.length << ", "
        << t.type << ", "
        << t.value << "}";
}


Token::Token(Loc loc, int length, TokenType type, int64_t value)
    : loc(loc), length(length), type(type), value(value) {}

bool Token::operator==(const Token& other) const {
    return loc == other.loc
        && length == other.length
        && type == other.type
        && value == other.value;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

bool Token::is_error() const {
    switch(type) {
        case TokenType::ERROR:
        case TokenType::ERROR_INT:
        case TokenType::ERROR_EOF:
        case TokenType::ERROR_CHAR:
        case TokenType::ERROR_COLON:
        case TokenType::ERROR_COMMENT:
            return true;
        default:
            return false;
    }
}
