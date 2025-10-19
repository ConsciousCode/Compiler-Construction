#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace ast {

struct Arith;
using Expr = std::variant<int, std::string, std::shared_ptr<Arith>>;

struct Arith {
    enum class Op { ADD, SUB, MUL };

    Op op;
    Expr left;
    Expr right;
};

struct Not;
struct Logical;
struct Comparison;
using Cond = std::variant<bool, std::shared_ptr<Not>, std::shared_ptr<Logical>, std::shared_ptr<Comparison>>;

struct Not {
    Cond cond;
};

struct Logical {
    enum class Op { AND, OR };

    Op op;
    Cond left;
    Cond right;
};

struct Comparison {
    enum class Op { EQ, LT, LE, GT, GE };

    Op op;
    Arith left;
    Arith right;
};

struct Assign {
    std::string name;
    Expr value;

    Assign(std::string varName, Expr val) : name(varName), value(val) {}; 
};

struct If;
struct While;
struct Skip;
using Cmd = std::variant<Assign, If, While, Skip>;

struct Block {
    std::vector<Cmd> cmds;
};

struct If {
    Cond cond;
    Block then;
    Block el;
};

struct While {
    Cond cond;
    Block body;
};

struct Skip {};

}
