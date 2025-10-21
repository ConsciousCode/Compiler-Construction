#pragma once

#include <cstdint>
#include <stdint.h>

namespace whilelang {
    enum class Nonterm : uint16_t {
        Program,
        Block,
        Cmd,
        Assign,
        If,
        While,
        Skip,
        Term,
        Factor,
        Cond,
        Not,
        Comparison,
        
        Disjunction,
        Conjunction,

        AdditiveOp,
        RelOp,
        LogicalOp,

        ArithExpr, 
        BoolExpr,
    };

}
