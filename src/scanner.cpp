#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unordered_map>
#include <string_view>

#include "scanner.hpp"

Scanner::Scanner(const char *input) : cur(input), loc {1, 0, 0} {}

/**
 * Advance the iterator by the given number ignoring the line.
 */
void Scanner::advance(int by) {
    cur += by;
    loc.pos += by;
    loc.col += by;
}

/**
 * Advance the iterator to the next line.
 */
void Scanner::newline() {
    ++loc.line;
    loc.col = 0;
}

/**
 * Consume any leading whitespace, advancing the iterator.
 */
void Scanner::consume_space() {
    while(cur[0] != '\0') {
        switch(cur[0]) {
            case '\n':
                newline();
                break;

            case '\r':
            case '\f': case '\v':
            case '\t': case ' ':
                ++loc.col;
                break;

            default: return;
        }
        ++loc.pos;
        ++cur;
    }
}

/**
* Attempt to consume a comment. Stops at the end of the input if the comment
* is not closed.
*/
void Scanner::consume_comment() {
    // Single-line comment
    if(cur[0] == '-' && cur[1] == '-') {
        advance(2);
        while(cur[0] != '\0') {
            if(cur[0] == '\n') {
                newline();
                advance();
                break;
            }
            advance();
        }
        return;
    }

    // Multi-line comment
    if(cur[0] == '{' && cur[1] == '-') {
        advance(2);
        do {
            consume_space();
            if(cur[0] == '-' && cur[1] == '}') {
                advance(2);
                return;
            }
            advance();
        } while(cur[0] != '\0');
    }
}

/**
 * Character which can be part of the rest of an identifier
 */
bool isrest(char c) {
    return isalnum(c) || c == '_' || c == '\'';
}

/**
 * Map of keywords to token types
 */
static const std::unordered_map<std::string_view, TokenType> keywords = {
    {"and", TokenType::OP_AND},
    {"or", TokenType::OP_OR},
    {"not", TokenType::OP_NOT},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"if", TokenType::KW_IF},
    {"then", TokenType::KW_THEN},
    {"else", TokenType::KW_ELSE},
    {"fi", TokenType::KW_FI},
    {"while", TokenType::KW_WHILE},
    {"do", TokenType::KW_DO},
    {"od", TokenType::KW_OD},
    {"skip", TokenType::KW_SKIP},
};

Token Scanner::next_token() {
    // Consume leading space and comments
    while(true) {
        if(cur[0] == '\0') {
            return Token { loc, 0, TokenType::ERROR_EOF };
        }
        // Break if these don't advance the iterator
        auto pos = loc.pos;
        consume_space();
        consume_comment();
        if(pos == loc.pos) break;
    }

    // Cache because it'll change with scanning
    Loc tokloc = loc;

    // Check for identifiers
    if(std::isalpha(cur[0])) {
        // First consume the entire identifier to avoid false positives
        // on prefixes
        const char *start = cur;
        do {
            advance();
        } while(isrest(cur[0]));

        int len = cur - start;

        // Check if it's a keyword
        auto it = keywords.find(std::string_view(start, len));
        return Token {
            tokloc, len,
            it == keywords.end()? TokenType::IDENT : it->second
        };
    }

    // Check for operators
    switch(cur[0]) {
        case '+':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_ADD };
        case '-':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_SUB };
        case '*':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_MUL };

        case ':':
            if(cur[1] == '=') {
                advance(2);
                return Token { tokloc, 2, TokenType::OP_ASSIGN };
            }
            return Token { tokloc, 0, TokenType::ERROR_COLON };

        case '=':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_EQ };

        case '>':
            if(cur[1] == '=') {
                advance(2);
                return Token { tokloc, 2, TokenType::OP_GE };
            }
            advance(1);
            return Token { tokloc, 1, TokenType::OP_GT };
        case '<':
            if(cur[1] == '=') {
                advance(2);
                return Token { tokloc, 2, TokenType::OP_LE };
            }
            advance(1);
            return Token { tokloc, 1, TokenType::OP_LT };

        case ';':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_SEMI };

        case '[':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_LBRACK };
        case ']':
            advance(1);
            return Token { tokloc, 1, TokenType::OP_RBRACK };
    }

    // Check for a number
    char *end = nullptr;
    int64_t value = std::strtoll(cur, &end, 10);
    if(int len = end - cur) {
        advance(len);
        return Token {
            tokloc, len,
            std::isalpha(cur[0])
                ? TokenType::ERROR_INT
                : TokenType::INTEGER,
            value
        };
    }
    return Token { tokloc, 0, TokenType::ERROR_CHAR };
}
