#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace ast {

struct Arith;
using Expr = std::variant<int, std::string, Arith>;

struct Arith {
    enum class Op { ADD, SUB, MUL };

    Op op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Not;
struct Logical;
struct Comparison;
using Cond = std::variant<bool, Not, Logical, Comparison>;

struct Not {
    std::unique_ptr<Cond> cond;
};

struct Logical {
    enum class Op { AND, OR };

    Op op;
    std::unique_ptr<Cond> left;
    std::unique_ptr<Cond> right;
};

struct Comparison {
    enum class Op { EQ, LT, LE, GT, GE };

    Op op;
    std::unique_ptr<Arith> left;
    std::unique_ptr<Arith> right;
};

struct Assign {
    std::string name;
    std::unique_ptr<Arith> value;
};

struct If;
struct While;
struct Skip;
using Cmd = std::variant<Assign, If, While, Skip>;

struct Block {
    std::vector<std::unique_ptr<Cmd>> cmds;
};

struct If {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Block> then;
    std::unique_ptr<Block> el;
};

struct While {
    std::unique_ptr<Cond> cond;
    std::unique_ptr<Block> body;
};

struct Skip {};

}
