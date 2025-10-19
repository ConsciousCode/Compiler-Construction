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

	static Expr make(Op op, Expr l, Expr r){
		return Expr{ std::make_shared<Arith>(Arith{op, std::move(l), std::move(r)})};
	}
};

struct Not;
struct Logical;
struct Comparison;
using Cond = std::variant<bool, std::shared_ptr<Not>, std::shared_ptr<Logical>, std::shared_ptr<Comparison>>;

struct Not {
    Cond cond;

	static Cond make(Cond c){
		return Cond{std::make_shared<Not>(Not{std::move(c)})};
	}

};

struct Logical {
    enum class Op { AND, OR };

    Op op;
    Cond left;
    Cond right;

	static Cond make(Op op, Cond l, Cond r){
		return Cond{std::make_shared<Logical>(
			Logical{op, std::move(l), std::move(r)}
		)};
	}

};

struct Comparison {
    enum class Op { EQ, LT, LE, GT, GE };

    Op op;
    Expr left;
    Expr right;

	static Cond make(Op op, Expr l, Expr r){
		return Cond{std::make_shared<Comparison>(
			Comparison{op, std::move(l), std::move(r)}
		)};
	}

};

struct Assign {
    std::string name;
    Expr value;


	Assign(std::string ident, Expr v) : name(std::move(ident)), value(std::move(v)) {}
	Assign(std::string ident, int n) : Assign(std::move(ident), Expr{n}) {}
	Assign(std::string ident, const char* s) : Assign(std::move(ident), Expr{std::string{s}}) {}
	Assign(std::string ident, std::string s) : Assign(std::move(ident), Expr{std::move(s)}) {}
};

struct If;
struct While;
struct Skip;
using Cmd = std::variant<Assign, If, While, Skip>;

struct Block {
    std::vector<Cmd> cmds;

	Block() = default;

	explicit Block(std::vector<Cmd> c) : cmds(std::move(c)) {}
	
	template<typename... Ts>
	Block(Ts&&... ts) : cmds{ std::forward<Ts>(ts)... } {}
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
