#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ast {

class ASTNode {
  public:
};

class LabeledNode : public ASTNode {
  public:
    explicit LabeledNode(size_t l) : label(l){};

    size_t get_label() const {
        return label;
    }
  
  private:
    size_t label;
};

class Arith;
using Expr = std::variant<int, std::string, std::shared_ptr<Arith>>;

class Arith : public ASTNode {

  public:
    enum class Op { ADD, SUB, MUL };

    Op op;
    Expr left;
    Expr right;

    Arith(Op operation, Expr l, Expr r) : op(operation), left(l), right(r){};

    static Expr make(Op op, Expr l, Expr r) {
        return Expr{std::make_shared<Arith>(op, std::move(l), std::move(r))};
    }
};

class Not;
class Logical;
class Comparison;
class BoolConst;
using Cond =
    std::variant<std::shared_ptr<BoolConst>, std::shared_ptr<Not>, std::shared_ptr<Logical>, std::shared_ptr<Comparison>>;

class BoolConst : public LabeledNode {
  public:
    bool value;
    BoolConst(bool v, size_t l) : LabeledNode(l), value(v) {};
    static Cond make(bool v, size_t label) {
        return Cond{std::make_shared<BoolConst>(v, label)};
    }
};

class Not : public LabeledNode{
  public:
    Cond cond;

    Not(Cond c, size_t l) : LabeledNode(l), cond(c) {};

    static Cond make(Cond c, size_t label) {
        return Cond{std::make_shared<Not>(std::move(c), label)};
    }
};

class Logical : public LabeledNode {
  public:
    enum class Op { AND, OR };

    Op op;
    Cond left;
    Cond right;

    Logical(Op operation, Cond l, Cond r, size_t label) : LabeledNode(label), op(operation), left(l), right(r) {};

    static Cond make(Op op, Cond l, Cond r, size_t label) {
        return Cond{std::make_shared<Logical>(op, std::move(l), std::move(r), label)};
    }
};

class Comparison : public LabeledNode {
  public:
    enum class Op { EQ, LT, LE, GT, GE };

    Op op;
    Expr left;
    Expr right;

    Comparison(Op operation, Expr l, Expr r, size_t label) : LabeledNode(label), op(operation), left(l), right(r) {};

    static Cond make(Op op, Expr l, Expr r, size_t label) {
        return Cond{std::make_shared<Comparison>(op, std::move(l), std::move(r), label)};
    }
};

class Assign : public LabeledNode {
  public:
    std::string name;
    Expr value;

    Assign(std::string ident, Expr v, size_t l) : LabeledNode(l), name(std::move(ident)), value(std::move(v)) {}
    Assign(std::string ident, int n, size_t l) : Assign(std::move(ident), Expr{n}, l) {}
    Assign(std::string ident, const char* s, size_t l) : Assign(std::move(ident), Expr{std::string{s}}, l) {}
    Assign(std::string ident, std::string s, size_t l) : Assign(std::move(ident), Expr{std::move(s)}, l) {}
};

class If;
class While;
class Skip : public LabeledNode {
  public:
    Skip() = default;
    Skip(int l) : LabeledNode(l){};
};
using Cmd = std::variant<Assign, If, While, Skip>;

class Block : public ASTNode {
  public:
    std::vector<Cmd> cmds;

    Block() = default;

    explicit Block(std::vector<Cmd> c) : cmds(std::move(c)) {}

    template <typename... Ts> Block(Ts&&... ts) {
        cmds.reserve(sizeof...(ts));
        (cmds.emplace_back(std::forward<Ts>(ts)), ...);
    }
};

class If : public ASTNode {
  public:
    Cond cond;
    Block then;
    Block el;

    If(Cond c, Block t, Block e) : cond(std::move(c)), then(std::move(t)), el(std::move(e)){};
};

class While : public ASTNode {
  public:
    Cond cond;
    Block body;

    While(Cond c, Block b) : cond(std::move(c)), body(std::move(b)){};
};

class Skip : public ASTNode {};

} // namespace ast
