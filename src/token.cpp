#include <ostream>

#include "token.hpp"

std::ostream& operator<<(std::ostream& os, const Loc& loc) {
    return os
        << loc.line << ":"
        << loc.col << "|"
        << loc.pos;
}

const char *to_string(TokenType tt) {
    switch(tt) {
        case TokenType::ERROR: return "ERROR";
        case TokenType::ERROR_INT: return "ERROR_INT";
        case TokenType::ERROR_EOF: return "EOF";
        case TokenType::ERROR_CHAR: return "ERROR_CHAR";
        case TokenType::ERROR_COLON: return "ERROR_COLON";
        case TokenType::IDENT: return "IDENT";
        case TokenType::INTEGER: return "INTEGER";

        case TokenType::KW_TRUE: return "'true'";
        case TokenType::KW_FALSE: return "'false'";
        case TokenType::KW_IF: return "'if'";
        case TokenType::KW_THEN: return "'then'";
        case TokenType::KW_ELSE: return "'else'";
        case TokenType::KW_FI: return "'fi'";
        case TokenType::KW_WHILE: return "'while'";
        case TokenType::KW_DO: return "'do'";
        case TokenType::KW_OD: return "'od'";
        case TokenType::KW_SKIP: return "'skip'";

        case TokenType::OP_ADD: return "+";
        case TokenType::OP_SUB: return "-";
        case TokenType::OP_MUL: return "*";

        case TokenType::OP_NOT: return "'not'";
        case TokenType::OP_AND: return "'and'";
        case TokenType::OP_OR: return "'or'";

        case TokenType::OP_EQ: return "'='";
        case TokenType::OP_LT: return "'<'";
        case TokenType::OP_GT: return "'>'";
        case TokenType::OP_LE: return "'<='";
        case TokenType::OP_GE: return "'>='";

        case TokenType::OP_ASSIGN: return "':='";
        case TokenType::OP_SEMI: return "';'";
        case TokenType::OP_LPAREN: return "'('";
        case TokenType::OP_RPAREN: return "')'";
        case TokenType::OP_LBRACK: return "'['";
        case TokenType::OP_RBRACK: return "']'";

        default: return "<unknown>";
    }
}

std::ostream& operator<<(std::ostream& os, const TokenType& tt) {
    return os << to_string(tt);
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
    return os << "Token("
        << t.type << ", "
        << t.length << ", "
        << "(" << t.loc << "), "
        << t.value << ")";
}
