#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ast {

class ASTNode {
  public:
};


class Arith;
using Expr = std::variant<int, std::string, std::shared_ptr<Arith>>;

class Arith : public ASTNode{

    public:
        enum class Op { ADD, SUB, MUL };

        Op op;
        Expr left;
        Expr right;

    Arith(Op operation, Expr l, Expr r) : op(operation), left(l) ,right(r) {};

    static Expr make(Op op, Expr l, Expr r) {
        return Expr{std::make_shared<Arith>(op, std::move(l), std::move(r))};
    }
};

class Not;
class Logical;
class Comparison;
using Cond =
    std::variant<bool, std::shared_ptr<Not>, std::shared_ptr<Logical>, std::shared_ptr<Comparison>>;

class Not : public ASTNode{
public:
    Cond cond;

    Not(Cond c) : cond(c) {};

    static Cond make(Cond c) {
        return Cond{std::make_shared<Not>(std::move(c))};
    }
};

class Logical : public ASTNode{
public:
    enum class Op { AND, OR };

    Op op;
    Cond left;
    Cond right;

    Logical(Op operation, Cond l, Cond r) : op(operation), left(l), right(r) {}; 

    static Cond make(Op op, Cond l, Cond r) {
        return Cond{std::make_shared<Logical>(op, std::move(l), std::move(r))};
    }
};

class Comparison : public ASTNode{
public:
    enum class Op { EQ, LT, LE, GT, GE };

    Op op;
    Expr left;
    Expr right;

    Comparison(Op operation, Expr l, Expr r) : op(operation), left(l) ,right(r) {};

    static Cond make(Op op, Expr l, Expr r) {
        return Cond{std::make_shared<Comparison>(Comparison{op, std::move(l), std::move(r)})};
    }
};

class Assign : public ASTNode{
public:
    std::string name;
    Expr value;

    Assign(std::string ident, Expr v) : name(std::move(ident)), value(std::move(v)) {}
    Assign(std::string ident, int n) : Assign(std::move(ident), Expr{n}) {}
    Assign(std::string ident, const char* s) : Assign(std::move(ident), Expr{std::string{s}}) {}
    Assign(std::string ident, std::string s) : Assign(std::move(ident), Expr{std::move(s)}) {}
};

class If;
class While;
class Skip;
using Cmd = std::variant<Assign, If, While, Skip>;

class Block : public ASTNode{
public:
    std::vector<Cmd> cmds;

    Block() = default;

    explicit Block(std::vector<Cmd> c) : cmds(std::move(c)) {}

    template <typename... Ts> 
    Block(Ts&&... ts) {
        cmds.reserve(sizeof...(ts));
        (cmds.emplace_back(std::forward<Ts>(ts)), ...);
    }
};

class If : public ASTNode{
public:
    Cond cond;
    Block then;
    Block el;

    If(Cond c, Block t, Block e) 
        : cond(std::move(c)), then(std::move(t)), el(std::move(e)) {};
};

class While : public ASTNode{
public:
    Cond cond;
    Block body;

    While(Cond c, Block b) 
        : cond(std::move(c)), body(std::move(b)) {}; 
};

class Skip : public ASTNode{};

} // namespace ast
