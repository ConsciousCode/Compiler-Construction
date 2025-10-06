#include <string>
#include <fstream>
#include <sstream>
#include <gtest/gtest.h>

#include "scanner.hpp"

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


static void read_file_or_fail(const std::string& path, std::string& out) {
    std::ifstream in(path);
    ASSERT_TRUE(in.good()) << "Cannot open file: " << path;
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
}

TEST(ScannerTest, Collatz_NoErrors) {
    std::string path = std::string(EXAMPLES_DIR) + "/collatz.while";
    std::string source;

    read_file_or_fail(path, source);
    
    Scanner s(source.c_str());
    for (;;) {
        Token t = s.next_token();

        ASSERT_NE(t.type, TokenType::ERROR) << "Scanner produced ERROR at " << t.loc;
        if (t.type == TokenType::TOK_EOF) 
        {
            break;
        }
    }
}

TEST(ScannerTest, Factorial_NoErrors) {
    std::string path = std::string(EXAMPLES_DIR) + "/factorial.while";
    std::string source;

    read_file_or_fail(path, source);
    Scanner s(source.c_str());
    for (;;) {
        Token t = s.next_token();

        ASSERT_NE(t.type, TokenType::ERROR) << "Scanner produced ERROR at " << t.loc;
        if (t.type ==TokenType::TOK_EOF)
        {
            break;
        }
    }
}

TEST(ScannerTest, Fibonacci_NoErrors) {
    std::string path = std::string(EXAMPLES_DIR) + "/fibonacci.while";
    std::string source;

    read_file_or_fail(path, source);
    Scanner s(source.c_str());
    for (;;) {
        Token t = s.next_token();

        ASSERT_NE(t.type, TokenType::ERROR) << "Scanner produced ERROR at " << t.loc;
        if (t.type ==TokenType::TOK_EOF)
        {
            break;
        }
    }
}