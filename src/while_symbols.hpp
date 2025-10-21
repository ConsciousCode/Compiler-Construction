#pragma once

#include <cstdint>
#include <stdint.h>
#include <vector>
#include "token.hpp"

namespace while_symbols {
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

    enum class SymKind : uint8_t {Terminal, NonTerminal, Epsilon, End }; 

    struct Symbol {
        SymKind kind;
        uint16_t id;
    };

    inline Symbol T(TokenType t)   { return {SymKind::Terminal,    static_cast<uint16_t>(t)}; }
    inline Symbol N(Nonterm nt)    { return {SymKind::NonTerminal, static_cast<uint16_t>(nt)}; }
    constexpr Symbol EPS{SymKind::Epsilon, 0};
    constexpr Symbol END{SymKind::End, 0};

    struct Production {
        Nonterm lhs;
        std::vector<Symbol> rhs;
    };
}
