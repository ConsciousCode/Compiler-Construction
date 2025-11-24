#include "../scanner.hpp"

#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

TEST(ScannerTest, SimpleIfStatement) {

    std::array<Token, 11> expected = {
        Token{Loc{1, 0, 0}, 2, TokenType::KW_IF, 0},
        Token{Loc{1, 3, 3}, 4, TokenType::KW_TRUE, 0},
        Token{Loc{1, 8, 8}, 4, TokenType::KW_THEN, 0},
        Token{Loc{2, 2, 15}, 4, TokenType::KW_SKIP, 0},
        Token{Loc{3, 0, 20}, 4, TokenType::KW_ELSE, 0},
        Token{Loc{4, 2, 27}, 2, TokenType::INTEGER, 10},
        Token{Loc{4, 4, 29}, 1, TokenType::OP_SEMI, 0},
        Token{Loc{4, 6, 31}, 1, TokenType::IDENT, 0},
        Token{Loc{4, 8, 33}, 2, TokenType::OP_ASSIGN, 0},
        Token{Loc{4, 11, 36}, 2, TokenType::INTEGER, 10},
        Token{Loc{5, 0, 39}, 2, TokenType::KW_FI, 0},
    };
    std::string input = "if true then\n  skip\nelse\n  10; x := 10\nfi";
    Scanner s = Scanner(input.c_str());

    std::vector<Token> actual;
    for (Token tok = s.next_token();
         tok.type != TokenType::ERROR_EOF && tok.type != TokenType::ERROR; tok = s.next_token()) {
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

static void test_file(const std::string& path, std::vector<Token> expected) {
    std::string source;
    read_file_or_fail(std::string(EXAMPLES_DIR) + path, source);
    Scanner s(source.c_str());
    for (size_t i = 0; i < expected.size(); ++i) {
        Token scanned = s.next_token();
        ASSERT_EQ(scanned, expected[i]) << "mismatch at expected[" << i << "]";
    }
}

TEST(ScannerTest, Collatz_NoErrors) {
    test_file("/collatz.while", {Token{Loc{6, 0, 95}, 1, TokenType::IDENT, 0},
                                 Token{Loc{6, 2, 97}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{6, 5, 100}, 5, TokenType::IDENT, 0},
                                 Token{Loc{6, 10, 105}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{7, 0, 107}, 5, TokenType::IDENT, 0},
                                 Token{Loc{7, 6, 113}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{7, 9, 116}, 1, TokenType::INTEGER, 0},
                                 Token{Loc{7, 10, 117}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{8, 0, 119}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{8, 6, 125}, 1, TokenType::IDENT, 0},
                                 Token{Loc{8, 8, 127}, 1, TokenType::OP_GT, 0},
                                 Token{Loc{8, 10, 129}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{8, 12, 131}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{9, 2, 136}, 3, TokenType::IDENT, 0},
                                 Token{Loc{9, 6, 140}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{9, 9, 143}, 1, TokenType::IDENT, 0},
                                 Token{Loc{9, 10, 144}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{10, 2, 148}, 4, TokenType::IDENT, 0},
                                 Token{Loc{10, 7, 153}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{10, 10, 156}, 1, TokenType::INTEGER, 0},
                                 Token{Loc{10, 11, 157}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{11, 2, 161}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{11, 8, 167}, 3, TokenType::IDENT, 0},
                                 Token{Loc{11, 12, 171}, 1, TokenType::OP_GT, 0},
                                 Token{Loc{11, 14, 173}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{11, 16, 175}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{12, 4, 182}, 3, TokenType::IDENT, 0},
                                 Token{Loc{12, 8, 186}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{12, 11, 189}, 3, TokenType::IDENT, 0},
                                 Token{Loc{12, 15, 193}, 1, TokenType::OP_SUB, 0},
                                 Token{Loc{12, 17, 195}, 1, TokenType::INTEGER, 2},
                                 Token{Loc{12, 18, 196}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{13, 4, 202}, 4, TokenType::IDENT, 0},
                                 Token{Loc{13, 9, 207}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{13, 12, 210}, 4, TokenType::IDENT, 0},
                                 Token{Loc{13, 17, 215}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{13, 19, 217}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{14, 2, 221}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{14, 4, 223}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{15, 2, 227}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{15, 5, 230}, 3, TokenType::IDENT, 0},
                                 Token{Loc{15, 9, 234}, 1, TokenType::OP_EQ, 0},
                                 Token{Loc{15, 11, 236}, 1, TokenType::INTEGER, 0},
                                 Token{Loc{15, 13, 238}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{16, 4, 247}, 1, TokenType::IDENT, 0},
                                 Token{Loc{16, 6, 249}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{16, 9, 252}, 4, TokenType::IDENT, 0},
                                 Token{Loc{17, 2, 259}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{18, 4, 268}, 1, TokenType::IDENT, 0},
                                 Token{Loc{18, 6, 270}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{18, 9, 273}, 1, TokenType::INTEGER, 3},
                                 Token{Loc{18, 10, 274}, 1, TokenType::OP_MUL, 0},
                                 Token{Loc{18, 11, 275}, 1, TokenType::IDENT, 0},
                                 Token{Loc{18, 12, 276}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{18, 13, 277}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{19, 2, 281}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{19, 4, 283}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{20, 2, 287}, 5, TokenType::IDENT, 0},
                                 Token{Loc{20, 8, 293}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{20, 11, 296}, 5, TokenType::IDENT, 0},
                                 Token{Loc{20, 17, 302}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{20, 19, 304}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{21, 0, 306}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{21, 2, 308}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{22, 0, 310}, 6, TokenType::IDENT, 0},
                                 Token{Loc{22, 7, 317}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{22, 10, 320}, 5, TokenType::IDENT, 0}});
}

TEST(ScannerTest, Comment_NoErrors) {
    test_file("/comment.while", {});
}

TEST(ScannerTest, Factorial_NoErrors) {
    test_file("/factorial.while", {Token{Loc{5, 0, 84}, 1, TokenType::IDENT, 0},
                                   Token{Loc{5, 2, 86}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{5, 5, 89}, 1, TokenType::IDENT, 0},
                                   Token{Loc{5, 6, 90}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{6, 0, 92}, 1, TokenType::IDENT, 0},
                                   Token{Loc{6, 2, 94}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{6, 5, 97}, 1, TokenType::INTEGER, 1},
                                   Token{Loc{6, 6, 98}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{7, 0, 100}, 5, TokenType::KW_WHILE, 0},
                                   Token{Loc{7, 6, 106}, 1, TokenType::IDENT, 0},
                                   Token{Loc{7, 8, 108}, 1, TokenType::OP_GT, 0},
                                   Token{Loc{7, 10, 110}, 1, TokenType::INTEGER, 1},
                                   Token{Loc{7, 12, 112}, 2, TokenType::KW_DO, 0},
                                   Token{Loc{8, 2, 117}, 1, TokenType::IDENT, 0},
                                   Token{Loc{8, 4, 119}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{8, 7, 122}, 1, TokenType::IDENT, 0},
                                   Token{Loc{8, 9, 124}, 1, TokenType::OP_MUL, 0},
                                   Token{Loc{8, 11, 126}, 1, TokenType::IDENT, 0},
                                   Token{Loc{8, 12, 127}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{9, 2, 131}, 1, TokenType::IDENT, 0},
                                   Token{Loc{9, 4, 133}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{9, 7, 136}, 1, TokenType::IDENT, 0},
                                   Token{Loc{9, 9, 138}, 1, TokenType::OP_SUB, 0},
                                   Token{Loc{9, 11, 140}, 1, TokenType::INTEGER, 1},
                                   Token{Loc{10, 0, 142}, 2, TokenType::KW_OD, 0},
                                   Token{Loc{10, 2, 144}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{11, 0, 146}, 1, TokenType::IDENT, 0},
                                   Token{Loc{11, 2, 148}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{11, 5, 151}, 1, TokenType::INTEGER, 0},
                                   Token{Loc{11, 6, 152}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{12, 0, 154}, 6, TokenType::IDENT, 0},
                                   Token{Loc{12, 7, 161}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{12, 10, 164}, 1, TokenType::IDENT, 0}});
}

TEST(ScannerTest, Fibonacci_NoErrors) {
    test_file("/fibonacci.while", {Token{Loc{7, 0, 85}, 1, TokenType::IDENT, 0},
                                   Token{Loc{7, 2, 87}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{7, 5, 90}, 1, TokenType::INTEGER, 0},
                                   Token{Loc{7, 6, 91}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{8, 0, 93}, 1, TokenType::IDENT, 0},
                                   Token{Loc{8, 2, 95}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{8, 5, 98}, 1, TokenType::INTEGER, 1},
                                   Token{Loc{8, 6, 99}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{9, 0, 101}, 1, TokenType::IDENT, 0},
                                   Token{Loc{9, 2, 103}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{9, 5, 106}, 1, TokenType::INTEGER, 0},
                                   Token{Loc{9, 6, 107}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{10, 0, 109}, 5, TokenType::KW_WHILE, 0},
                                   Token{Loc{10, 6, 115}, 3, TokenType::OP_NOT, 0},
                                   Token{Loc{10, 9, 118}, 1, TokenType::OP_LBRACK, 0},
                                   Token{Loc{10, 10, 119}, 1, TokenType::IDENT, 0},
                                   Token{Loc{10, 11, 120}, 1, TokenType::OP_EQ, 0},
                                   Token{Loc{10, 12, 121}, 1, TokenType::IDENT, 0},
                                   Token{Loc{10, 13, 122}, 1, TokenType::OP_RBRACK, 0},
                                   Token{Loc{10, 15, 124}, 2, TokenType::KW_DO, 0},
                                   Token{Loc{11, 2, 129}, 1, TokenType::IDENT, 0},
                                   Token{Loc{11, 4, 131}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{11, 7, 134}, 1, TokenType::IDENT, 0},
                                   Token{Loc{11, 8, 135}, 1, TokenType::OP_ADD, 0},
                                   Token{Loc{11, 9, 136}, 1, TokenType::IDENT, 0},
                                   Token{Loc{11, 10, 137}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{12, 2, 141}, 1, TokenType::IDENT, 0},
                                   Token{Loc{12, 4, 143}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{12, 7, 146}, 1, TokenType::IDENT, 0},
                                   Token{Loc{12, 8, 147}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{13, 2, 151}, 1, TokenType::IDENT, 0},
                                   Token{Loc{13, 4, 153}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{13, 7, 156}, 1, TokenType::IDENT, 0},
                                   Token{Loc{13, 8, 157}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{14, 2, 161}, 1, TokenType::IDENT, 0},
                                   Token{Loc{14, 4, 163}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{14, 7, 166}, 1, TokenType::IDENT, 0},
                                   Token{Loc{14, 8, 167}, 1, TokenType::OP_SUB, 0},
                                   Token{Loc{14, 9, 168}, 1, TokenType::INTEGER, 1},
                                   Token{Loc{14, 10, 169}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{15, 2, 173}, 1, TokenType::IDENT, 0},
                                   Token{Loc{15, 4, 175}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{15, 7, 178}, 1, TokenType::INTEGER, 0},
                                   Token{Loc{16, 0, 180}, 2, TokenType::KW_OD, 0},
                                   Token{Loc{16, 2, 182}, 1, TokenType::OP_SEMI, 0},
                                   Token{Loc{17, 0, 184}, 6, TokenType::IDENT, 0},
                                   Token{Loc{17, 7, 191}, 2, TokenType::OP_ASSIGN, 0},
                                   Token{Loc{17, 10, 194}, 1, TokenType::IDENT, 0}});
}

TEST(ScannerTest, IsPrime_NoErrors) {
    test_file("/isprime.while", {Token{Loc{7, 0, 100}, 9, TokenType::IDENT, 0},
                                 Token{Loc{7, 10, 110}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{7, 13, 113}, 1, TokenType::INTEGER, 0},
                                 Token{Loc{7, 14, 114}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{8, 0, 116}, 1, TokenType::IDENT, 0},
                                 Token{Loc{8, 2, 118}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{8, 5, 121}, 1, TokenType::INTEGER, 2},
                                 Token{Loc{8, 6, 122}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{9, 0, 124}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{9, 6, 130}, 1, TokenType::IDENT, 0},
                                 Token{Loc{9, 8, 132}, 1, TokenType::OP_LT, 0},
                                 Token{Loc{9, 10, 134}, 1, TokenType::IDENT, 0},
                                 Token{Loc{9, 12, 136}, 3, TokenType::OP_AND, 0},
                                 Token{Loc{9, 16, 140}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{9, 20, 144}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{9, 21, 145}, 9, TokenType::IDENT, 0},
                                 Token{Loc{9, 30, 154}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{9, 32, 156}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{10, 4, 163}, 1, TokenType::IDENT, 0},
                                 Token{Loc{10, 6, 165}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{10, 9, 168}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{10, 10, 169}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{11, 4, 175}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{11, 10, 181}, 1, TokenType::IDENT, 0},
                                 Token{Loc{11, 11, 182}, 1, TokenType::OP_MUL, 0},
                                 Token{Loc{11, 12, 183}, 1, TokenType::IDENT, 0},
                                 Token{Loc{11, 14, 185}, 1, TokenType::OP_LT, 0},
                                 Token{Loc{11, 16, 187}, 1, TokenType::IDENT, 0},
                                 Token{Loc{11, 18, 189}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{12, 8, 200}, 1, TokenType::IDENT, 0},
                                 Token{Loc{12, 10, 202}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{12, 11, 203}, 1, TokenType::OP_EQ, 0},
                                 Token{Loc{12, 13, 205}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{13, 4, 211}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{13, 6, 213}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{14, 4, 219}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{14, 7, 222}, 1, TokenType::IDENT, 0},
                                 Token{Loc{14, 8, 223}, 1, TokenType::OP_MUL, 0},
                                 Token{Loc{14, 9, 224}, 1, TokenType::IDENT, 0},
                                 Token{Loc{14, 11, 226}, 1, TokenType::OP_EQ, 0},
                                 Token{Loc{14, 12, 227}, 1, TokenType::OP_EQ, 0},
                                 Token{Loc{14, 14, 229}, 1, TokenType::IDENT, 0},
                                 Token{Loc{14, 16, 231}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{15, 8, 244}, 9, TokenType::IDENT, 0},
                                 Token{Loc{15, 18, 254}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{15, 21, 257}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{16, 4, 263}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{17, 8, 276}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 10, 278}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{17, 13, 281}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 15, 283}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{17, 17, 285}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{18, 4, 291}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{19, 0, 294}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{19, 2, 296}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{20, 0, 298}, 6, TokenType::IDENT, 0},
                                 Token{Loc{20, 7, 305}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{20, 10, 308}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{20, 12, 310}, 1, TokenType::OP_SUB, 0},
                                 Token{Loc{20, 14, 312}, 9, TokenType::IDENT, 0}});
}

TEST(ScannerTest, Loop_NoError) {
    test_file("/loop.while", {Token{Loc{7, 0, 71}, 5, TokenType::KW_WHILE, 0},
                              Token{Loc{7, 6, 77}, 1, TokenType::IDENT, 0},
                              Token{Loc{7, 8, 79}, 1, TokenType::OP_GT, 0},
                              Token{Loc{7, 10, 81}, 1, TokenType::INTEGER, 0},
                              Token{Loc{7, 12, 83}, 2, TokenType::KW_DO, 0},
                              Token{Loc{8, 2, 88}, 6, TokenType::IDENT, 0},
                              Token{Loc{8, 9, 95}, 2, TokenType::OP_ASSIGN, 0},
                              Token{Loc{8, 12, 98}, 6, TokenType::IDENT, 0},
                              Token{Loc{8, 19, 105}, 1, TokenType::OP_ADD, 0},
                              Token{Loc{8, 21, 107}, 1, TokenType::INTEGER, 1},
                              Token{Loc{9, 2, 111}, 1, TokenType::IDENT, 0},
                              Token{Loc{9, 4, 113}, 2, TokenType::OP_ASSIGN, 0},
                              Token{Loc{9, 7, 116}, 1, TokenType::IDENT, 0},
                              Token{Loc{9, 9, 118}, 1, TokenType::OP_SUB, 0},
                              Token{Loc{9, 11, 120}, 1, TokenType::INTEGER, 1},
                              Token{Loc{10, 0, 122}, 2, TokenType::KW_OD, 0}});
}

TEST(ScannerTest, Torture_NoError) {
    test_file("/torture.while", {Token{Loc{1, 0, 0}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{1, 3, 3}, 4, TokenType::KW_TRUE, 0},
                                 Token{Loc{1, 8, 8}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{1, 13, 13}, 1, TokenType::IDENT, 0},
                                 Token{Loc{1, 14, 14}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{1, 18, 18}, 1, TokenType::INTEGER, 2},
                                 Token{Loc{1, 20, 20}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{2, 2, 27}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{2, 5, 30}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{2, 9, 34}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{2, 10, 35}, 3, TokenType::IDENT, 0},
                                 Token{Loc{2, 13, 38}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{2, 15, 40}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{2, 20, 45}, 3, TokenType::IDENT, 0},
                                 Token{Loc{2, 24, 49}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{2, 27, 52}, 5, TokenType::IDENT, 0},
                                 Token{Loc{2, 33, 58}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{2, 38, 63}, 5, TokenType::IDENT, 0},
                                 Token{Loc{2, 44, 69}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{2, 47, 72}, 3, TokenType::IDENT, 0},
                                 Token{Loc{3, 0, 76}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{3, 3, 79}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{3, 17, 93}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{4, 0, 107}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{4, 4, 111}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{4, 6, 113}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{4, 11, 118}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{4, 13, 120}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{4, 18, 125}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{4, 19, 126}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{4, 23, 130}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{4, 25, 132}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{4, 29, 136}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{5, 0, 138}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{5, 3, 141}, 1, TokenType::IDENT, 0},
                                 Token{Loc{6, 4, 157}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{6, 9, 162}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{6, 12, 165}, 1, TokenType::IDENT, 0},
                                 Token{Loc{6, 14, 167}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{9, 15, 239}, 1, TokenType::IDENT, 0},
                                 Token{Loc{9, 16, 240}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{9, 18, 242}, 1, TokenType::OP_LPAREN, 0},
                                 Token{Loc{9, 19, 243}, 1, TokenType::OP_LPAREN, 0},
                                 Token{Loc{9, 20, 244}, 1, TokenType::OP_LPAREN, 0},
                                 Token{Loc{9, 21, 245}, 1, TokenType::IDENT, 0},
                                 Token{Loc{9, 22, 246}, 1, TokenType::OP_RPAREN, 0},
                                 Token{Loc{9, 23, 247}, 1, TokenType::OP_RPAREN, 0},
                                 Token{Loc{9, 24, 248}, 1, TokenType::OP_RPAREN, 0},
                                 Token{Loc{10, 4, 269}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{10, 9, 274}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{11, 2, 281}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{11, 5, 284}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{11, 10, 289}, 1, TokenType::IDENT, 0},
                                 Token{Loc{11, 12, 291}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{11, 15, 294}, 5, TokenType::IDENT, 0},
                                 Token{Loc{11, 21, 300}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{11, 23, 302}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{12, 0, 304}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{12, 3, 307}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{12, 4, 308}, 5, TokenType::KW_FALSE, 0},
                                 Token{Loc{12, 9, 313}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{12, 11, 315}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{12, 16, 320}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{13, 0, 325}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{13, 5, 330}, 5, TokenType::IDENT, 0},
                                 Token{Loc{13, 11, 336}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{13, 14, 339}, 1, TokenType::OP_LPAREN, 0},
                                 Token{Loc{13, 15, 340}, 5, TokenType::IDENT, 0},
                                 Token{Loc{13, 20, 345}, 1, TokenType::OP_RPAREN, 0},
                                 Token{Loc{13, 21, 346}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{14, 1, 349}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{14, 6, 354}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{16, 0, 391}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{16, 6, 397}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{16, 7, 398}, 4, TokenType::KW_TRUE, 0},
                                 Token{Loc{16, 11, 402}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{16, 13, 404}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{17, 4, 411}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 6, 413}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{17, 9, 416}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 11, 418}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{17, 13, 420}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 15, 422}, 1, TokenType::OP_MUL, 0},
                                 Token{Loc{17, 17, 424}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 19, 426}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{17, 21, 428}, 1, TokenType::IDENT, 0},
                                 Token{Loc{17, 22, 429}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{18, 4, 435}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{18, 8, 439}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{19, 4, 445}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{19, 7, 448}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{19, 8, 449}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{19, 12, 453}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{19, 14, 455}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{19, 17, 458}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{19, 18, 459}, 1, TokenType::IDENT, 0},
                                 Token{Loc{19, 26, 467}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{19, 27, 468}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{19, 28, 469}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{19, 30, 471}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{20, 8, 484}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{20, 14, 490}, 5, TokenType::KW_FALSE, 0},
                                 Token{Loc{21, 8, 511}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{22, 12, 526}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{22, 18, 532}, 1, TokenType::IDENT, 0},
                                 Token{Loc{22, 20, 534}, 1, TokenType::OP_GT, 0},
                                 Token{Loc{22, 22, 536}, 1, TokenType::INTEGER, 3},
                                 Token{Loc{22, 24, 538}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{24, 0, 559}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{24, 5, 564}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{25, 8, 575}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{25, 10, 577}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{25, 12, 579}, 3, TokenType::IDENT, 0},
                                 Token{Loc{25, 16, 583}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{25, 19, 586}, 3, TokenType::IDENT, 0},
                                 Token{Loc{25, 23, 590}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{25, 25, 592}, 3, TokenType::IDENT, 0},
                                 Token{Loc{25, 28, 595}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{25, 30, 597}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{26, 4, 606}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{26, 9, 611}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{27, 0, 616}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{27, 3, 619}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{28, 0, 622}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{28, 3, 625}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{28, 4, 626}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{28, 6, 628}, 1, TokenType::OP_GT, 0},
                                 Token{Loc{28, 8, 630}, 1, TokenType::INTEGER, 2},
                                 Token{Loc{28, 10, 632}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{28, 12, 634}, 1, TokenType::INTEGER, 3},
                                 Token{Loc{28, 14, 636}, 1, TokenType::OP_MUL, 0},
                                 Token{Loc{28, 16, 638}, 1, TokenType::OP_LPAREN, 0},
                                 Token{Loc{28, 17, 639}, 1, TokenType::INTEGER, 4},
                                 Token{Loc{28, 19, 641}, 1, TokenType::OP_SUB, 0},
                                 Token{Loc{28, 21, 643}, 1, TokenType::INTEGER, 7},
                                 Token{Loc{28, 22, 644}, 1, TokenType::OP_RPAREN, 0},
                                 Token{Loc{28, 23, 645}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{28, 25, 647}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{28, 30, 652}, 2, TokenType::IDENT, 0},
                                 Token{Loc{28, 33, 655}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{28, 36, 658}, 1, TokenType::INTEGER, 1},
                                 Token{Loc{28, 38, 660}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{28, 43, 665}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{28, 48, 670}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{29, 0, 673}, 2, TokenType::KW_IF, 0},
                                 Token{Loc{29, 3, 676}, 1, TokenType::IDENT, 0},
                                 Token{Loc{29, 5, 678}, 3, TokenType::OP_AND, 0},
                                 Token{Loc{29, 9, 682}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{29, 12, 685}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 13, 686}, 3, TokenType::IDENT, 0},
                                 Token{Loc{29, 16, 689}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{29, 17, 690}, 2, TokenType::OP_OR, 0},
                                 Token{Loc{29, 19, 692}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 20, 693}, 4, TokenType::KW_TRUE, 0},
                                 Token{Loc{29, 24, 697}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{29, 26, 699}, 3, TokenType::OP_AND, 0},
                                 Token{Loc{29, 30, 703}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 31, 704}, 5, TokenType::KW_FALSE, 0},
                                 Token{Loc{29, 36, 709}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{29, 38, 711}, 3, TokenType::OP_AND, 0},
                                 Token{Loc{29, 41, 714}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 42, 715}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 43, 716}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{29, 46, 719}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 47, 720}, 7, TokenType::IDENT, 0},
                                 Token{Loc{29, 55, 728}, 3, TokenType::OP_AND, 0},
                                 Token{Loc{29, 59, 732}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{29, 62, 735}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{29, 63, 736}, 4, TokenType::KW_TRUE, 0},
                                 Token{Loc{29, 67, 740}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{29, 68, 741}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{29, 69, 742}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{31, 8, 762}, 4, TokenType::KW_THEN, 0},
                                 Token{Loc{31, 13, 767}, 7, TokenType::IDENT, 0},
                                 Token{Loc{31, 21, 775}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{31, 24, 778}, 6, TokenType::IDENT, 0},
                                 Token{Loc{31, 31, 785}, 1, TokenType::OP_ADD, 0},
                                 Token{Loc{31, 33, 787}, 10, TokenType::IDENT, 0},
                                 Token{Loc{31, 44, 798}, 4, TokenType::KW_ELSE, 0},
                                 Token{Loc{32, 4, 807}, 3, TokenType::IDENT, 0},
                                 Token{Loc{32, 8, 811}, 2, TokenType::OP_ASSIGN, 0},
                                 Token{Loc{32, 11, 814}, 3, TokenType::IDENT, 0},
                                 Token{Loc{32, 14, 817}, 1, TokenType::OP_SEMI, 0},
                                 Token{Loc{32, 15, 818}, 5, TokenType::KW_WHILE, 0},
                                 Token{Loc{32, 21, 824}, 3, TokenType::OP_NOT, 0},
                                 Token{Loc{32, 24, 827}, 1, TokenType::OP_LBRACK, 0},
                                 Token{Loc{32, 30, 833}, 4, TokenType::KW_TRUE, 0},
                                 Token{Loc{32, 34, 837}, 1, TokenType::OP_RBRACK, 0},
                                 Token{Loc{32, 36, 839}, 2, TokenType::KW_DO, 0},
                                 Token{Loc{32, 39, 842}, 4, TokenType::KW_SKIP, 0},
                                 Token{Loc{32, 44, 847}, 2, TokenType::KW_OD, 0},
                                 Token{Loc{32, 47, 850}, 2, TokenType::KW_FI, 0},
                                 Token{Loc{33, 0, 853}, 2, TokenType::KW_FI, 0}});
}

TEST(ScannerTest, Colon_Error) {
    test_file("/scan_error/colon.while", {Token{Loc{1, 0, 0}, 1, TokenType::IDENT, 0},
                                          Token{Loc{1, 2, 2}, 0, TokenType::ERROR_COLON, 0}});
}

TEST(ScannerTest, Unclosed_Comment_Error) {
    test_file("/scan_error/unclosed_comment.while",
              {Token{Loc{1, 0, 0}, 52, TokenType::ERROR_COMMENT, 0}});
}

TEST(ScannerTest, Int_Alpha_Error) {
    test_file("/scan_error/int_alpha.while", {Token{Loc{1, 0, 0}, 1, TokenType::IDENT, 0},
                                              Token{Loc{1, 2, 2}, 2, TokenType::OP_ASSIGN, 0},
                                              Token{Loc{1, 5, 5}, 3, TokenType::ERROR_INT, 123}});
}

TEST(ScannerTest, Unicode_Error) {
    test_file("/scan_error/unicode.while", {Token{Loc{1, 0, 0}, 0, TokenType::ERROR_CHAR, 0}});
}

TEST(ScannerTest, Unprintable_Error) {
    test_file("/scan_error/unprintable.while", {Token{Loc{1, 4, 4}, 0, TokenType::ERROR_CHAR, 0}});
}

/// These tests are errors for the parser but not for the scanner

TEST(ScannerTest, ParserError_Assign_Bool_NoError) {
    test_file("/parse_error/assign_bool.while", {Token{Loc{1, 0, 0}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{1, 2, 2}, 2, TokenType::OP_ASSIGN, 0},
                                                 Token{Loc{1, 5, 5}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{1, 7, 7}, 1, TokenType::OP_GT, 0},
                                                 Token{Loc{1, 9, 9}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{1, 10, 10}, 1, TokenType::OP_SEMI, 0},
                                                 Token{Loc{2, 0, 12}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{2, 2, 14}, 2, TokenType::OP_ASSIGN, 0},
                                                 Token{Loc{2, 5, 17}, 4, TokenType::KW_TRUE, 0},
                                                 Token{Loc{2, 9, 21}, 1, TokenType::OP_SEMI, 0},
                                                 Token{Loc{3, 0, 23}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{3, 2, 25}, 2, TokenType::OP_ASSIGN, 0},
                                                 Token{Loc{3, 5, 28}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{3, 7, 30}, 1, TokenType::OP_EQ, 0},
                                                 Token{Loc{3, 9, 32}, 1, TokenType::IDENT, 0}});
}

TEST(ScannerTest, ParserError_Bare_Equals_NoError) {
    test_file("/parse_error/bare_equals.while", {Token{Loc{1, 0, 0}, 1, TokenType::IDENT, 0},
                                                 Token{Loc{1, 2, 2}, 1, TokenType::OP_EQ, 0},
                                                 Token{Loc{1, 4, 4}, 1, TokenType::INTEGER, 1}});
}

TEST(ScannerTest, ParserError_If_Semi_NoError) {
    test_file("/parse_error/if_semi.while", {
                                                Token{Loc{1, 0, 0}, 2, TokenType::KW_IF, 0},
                                                Token{Loc{1, 3, 3}, 5, TokenType::KW_FALSE, 0},
                                                Token{Loc{1, 9, 9}, 4, TokenType::KW_THEN, 0},
                                                Token{Loc{2, 4, 18}, 1, TokenType::IDENT, 0},
                                                Token{Loc{2, 6, 20}, 2, TokenType::OP_ASSIGN, 0},
                                                Token{Loc{2, 9, 23}, 1, TokenType::INTEGER, 1},
                                                Token{Loc{2, 10, 24}, 1, TokenType::OP_SEMI, 0},
                                                Token{Loc{3, 4, 30}, 1, TokenType::IDENT, 0},
                                                Token{Loc{3, 6, 32}, 2, TokenType::OP_ASSIGN, 0},
                                                Token{Loc{3, 9, 35}, 1, TokenType::INTEGER, 2},
                                                Token{Loc{3, 10, 36}, 1, TokenType::OP_SEMI, 0},
                                                Token{Loc{4, 0, 38}, 4, TokenType::KW_ELSE, 0},
                                                Token{Loc{5, 4, 47}, 1, TokenType::IDENT, 0},
                                                Token{Loc{5, 6, 49}, 2, TokenType::OP_ASSIGN, 0},
                                                Token{Loc{5, 9, 52}, 1, TokenType::INTEGER, 3},
                                                Token{Loc{5, 10, 53}, 1, TokenType::OP_SEMI, 0},
                                                Token{Loc{6, 0, 55}, 2, TokenType::KW_FI, 0},
                                            });
}

TEST(ScannerTest, ParserError_Missing_Else_NoError) {
    test_file("/parse_error/missing_else.while", {Token{Loc{1, 0, 0}, 2, TokenType::KW_IF, 0},
                                                  Token{Loc{1, 3, 3}, 4, TokenType::KW_TRUE, 0},
                                                  Token{Loc{1, 8, 8}, 4, TokenType::KW_THEN, 0},
                                                  Token{Loc{2, 4, 17}, 4, TokenType::KW_SKIP, 0},
                                                  Token{Loc{3, 0, 22}, 2, TokenType::KW_FI, 0}});
}

TEST(ScannerTest, ParserError_Missing_Fi_NoError) {
    test_file("/parse_error/missing_fi.while", {Token{Loc{1, 0, 0}, 2, TokenType::KW_IF, 0},
                                                Token{Loc{1, 3, 3}, 5, TokenType::KW_FALSE, 0},
                                                Token{Loc{1, 9, 9}, 4, TokenType::KW_THEN, 0},
                                                Token{Loc{2, 4, 18}, 4, TokenType::KW_SKIP, 0},
                                                Token{Loc{3, 0, 23}, 4, TokenType::KW_ELSE, 0},
                                                Token{Loc{4, 4, 32}, 1, TokenType::IDENT, 0},
                                                Token{Loc{4, 6, 34}, 2, TokenType::OP_ASSIGN, 0},
                                                Token{Loc{4, 9, 37}, 1, TokenType::INTEGER, 1}});
}

TEST(ScannerTest, ParserError_Not_NoError) {
    test_file("/parse_error/not.while", {Token{Loc{1, 0, 0}, 5, TokenType::KW_WHILE, 0},
                                         Token{Loc{1, 6, 6}, 3, TokenType::OP_NOT, 0},
                                         Token{Loc{1, 10, 10}, 4, TokenType::KW_TRUE, 0},
                                         Token{Loc{1, 15, 15}, 2, TokenType::KW_DO, 0},
                                         Token{Loc{2, 4, 22}, 4, TokenType::KW_SKIP, 0},
                                         Token{Loc{3, 0, 27}, 2, TokenType::KW_OD, 0}});
}

TEST(ScannerTest, ParserError_Not_If_NoError) {
    test_file("/parse_error/not_if.while", {Token{Loc{1, 0, 0}, 2, TokenType::KW_IF, 0},
                                            Token{Loc{1, 3, 3}, 3, TokenType::OP_NOT, 0},
                                            Token{Loc{1, 7, 7}, 5, TokenType::KW_FALSE, 0},
                                            Token{Loc{1, 13, 13}, 4, TokenType::KW_THEN, 0},
                                            Token{Loc{2, 4, 22}, 1, TokenType::IDENT, 0},
                                            Token{Loc{2, 6, 24}, 2, TokenType::OP_ASSIGN, 0},
                                            Token{Loc{2, 9, 27}, 1, TokenType::INTEGER, 1},
                                            Token{Loc{3, 0, 29}, 4, TokenType::KW_ELSE, 0},
                                            Token{Loc{4, 4, 38}, 1, TokenType::IDENT, 0},
                                            Token{Loc{4, 6, 40}, 2, TokenType::OP_ASSIGN, 0},
                                            Token{Loc{4, 9, 43}, 1, TokenType::INTEGER, 2},
                                            Token{Loc{5, 0, 45}, 2, TokenType::KW_FI, 0}});
}

TEST(ScannerTest, ParserError_Trailing_Semi_NoError) {
    test_file("/parse_error/trailing_semi.while", {Token{Loc{1, 0, 0}, 1, TokenType::IDENT, 0},
                                                   Token{Loc{1, 2, 2}, 2, TokenType::OP_ASSIGN, 0},
                                                   Token{Loc{1, 5, 5}, 1, TokenType::INTEGER, 1},
                                                   Token{Loc{1, 6, 6}, 1, TokenType::OP_SEMI, 0}});
}

TEST(ScannerTest, ParserError_While_Semi_NoError) {
    test_file("/parse_error/while_semi.while", {Token{Loc{1, 0, 0}, 5, TokenType::KW_WHILE, 0},
                                                Token{Loc{1, 6, 6}, 4, TokenType::KW_TRUE, 0},
                                                Token{Loc{1, 11, 11}, 2, TokenType::KW_DO, 0},
                                                Token{Loc{2, 4, 18}, 1, TokenType::IDENT, 0},
                                                Token{Loc{2, 6, 20}, 2, TokenType::OP_ASSIGN, 0},
                                                Token{Loc{2, 9, 23}, 1, TokenType::INTEGER, 1},
                                                Token{Loc{2, 10, 24}, 1, TokenType::OP_SEMI, 0},
                                                Token{Loc{3, 4, 30}, 1, TokenType::IDENT, 0},
                                                Token{Loc{3, 6, 32}, 2, TokenType::OP_ASSIGN, 0},
                                                Token{Loc{3, 9, 35}, 1, TokenType::INTEGER, 2},
                                                Token{Loc{3, 10, 36}, 1, TokenType::OP_SEMI, 0},
                                                Token{Loc{4, 0, 38}, 2, TokenType::KW_OD, 0}});
}
