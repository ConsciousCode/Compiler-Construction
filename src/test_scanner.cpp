#include <iostream>

#include "token.hpp"
#include "scanner.hpp"

void print_token(const Token &tok) {
    std::cout << tok << std::endl;
}

void test_scanner(const char *input) {
    std::cout << "Test scanner(\"" << input << "\")" << std::endl;
    Scanner s = Scanner(input);
    Token tok = s.next_token();
    while(tok.type != TokenType::TOK_EOF && tok.type != TokenType::ERROR) {
        print_token(tok);
        tok = s.next_token();
    }
}

int main() {
    test_scanner("1 2 3");
    test_scanner("if true then\n  skip\nelse\n  10; x := 10\nfi");
    return 0;
}
