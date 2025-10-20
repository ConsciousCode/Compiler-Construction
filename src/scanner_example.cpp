#include <iostream>
#include <fstream>

#include "token.hpp"
#include "scanner.hpp"

void print_token(const Token &tok) {
    std::cout << tok << std::endl;
}

void test_scanner(const char *input) {
    std::cout << input << std::endl << "---" << std::endl;
    Scanner s = Scanner(input);
    Token tok = s.next_token();
    while(tok.type != TokenType::ERROR_EOF) {
        print_token(tok);
        if(tok.type == TokenType::ERROR) break;
        tok = s.next_token();
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::ifstream ifs(argv[1]);
    std::string prog, line;
    while(std::getline(ifs, line)) {
        prog += line + "\n";
    }
    ifs.close();
    test_scanner(prog.c_str());
    return 0;
}
