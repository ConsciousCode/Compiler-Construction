#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "ast.hpp"

struct CodegenOutput {
    std::string riscv;
    std::vector<std::string> vars;
};

class Codegen {
    public:
        CodegenOutput generate(const ast::Block& program, std::string program_name);

    private:
        std::unordered_map<std::string, std::size_t> slot;
        std::vector<std::string> vars;

        void collect_vars(const ast::Block& b);
};