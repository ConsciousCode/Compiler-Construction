#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "scanner.hpp"

std::ostream& operator<<(std::ostream& os, const Loc& loc) {
    return os
        << loc.line << ":"
        << loc.col << "|"
        << loc.pos;
}

const char *to_string(TokenType tt) {
    switch(tt) {
        case TokenType::ERROR: return "ERROR";
        case TokenType::TOK_EOF: return "EOF";
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

TEST(ScannerTest, SimpleIfStatement){

    std::array<Token, 11> expected = {
        Token{Loc{1,0,0}, 2, TokenType::KW_IF, 0},
        Token{Loc{1,3,3}, 4, TokenType::KW_TRUE, 0},
        Token{Loc{1,8,8}, 4, TokenType::KW_THEN, 0},
        Token{Loc{2,2,15}, 4, TokenType::KW_SKIP, 0},
        Token{Loc{3,0,20}, 4, TokenType::KW_ELSE, 0},
        Token{Loc{4,2,27}, 2, TokenType::INTEGER, 10},
        Token{Loc{4,4,29}, 1, TokenType::OP_SEMI, 0},
        Token{Loc{4,6,31}, 1, TokenType::IDENT, 0},
        Token{Loc{4,8,33}, 2, TokenType::OP_ASSIGN, 0},
        Token{Loc{4,11,36}, 2, TokenType::INTEGER, 10},
        Token{Loc{5,0,39}, 2, TokenType::KW_FI, 0},
    };

    std::string input = "if true then\n  skip\nelse\n  10; x := 10\nfi";
    Scanner s = Scanner(input.c_str());

    std::vector<Token> actual;
    for (Token tok = s.next_token();
         tok.type != TokenType::TOK_EOF && tok.type != TokenType::ERROR;
         tok = s.next_token()) {
        actual.push_back(tok);
    }

    ASSERT_EQ(actual.size(), expected.size()) << "token count mismatch";
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i]) << "mismatch at token index " << i;
    }
}
