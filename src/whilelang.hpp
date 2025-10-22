#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>

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

inline std::string to_string(Nonterm nt) {
    switch (nt) {
    case Nonterm::Program:
        return "Program";
    case Nonterm::Block:
        return "Block";
    case Nonterm::Cmd:
        return "Command";
    case Nonterm::Assign:
        return "Assignment";
    case Nonterm::If:
        return "If Statement";
    case Nonterm::While:
        return "While Loop";
    case Nonterm::Skip:
        return "Skip";
    case Nonterm::Term:
        return "Term";
    case Nonterm::Factor:
        return "Factor";
    case Nonterm::Cond:
        return "Condition";
    case Nonterm::Not:
        return "Not";
    case Nonterm::Comparison:
        return "Comparison";
    case Nonterm::Disjunction:
        return "Disjunction";
    case Nonterm::Conjunction:
        return "Conjunction";
    case Nonterm::AdditiveOp:
        return "Additive Operator";
    case Nonterm::RelOp:
        return "Relational Operator";
    case Nonterm::LogicalOp:
        return "Logical Operator";
    case Nonterm::ArithExpr:
        return "Arithmetic Expression";
    case Nonterm::BoolExpr:
        return "Boolean Expression";
    default:
        return "Unknown";
    }
}

inline std::ostream& operator<<(std::ostream& os, Nonterm nt) {
    os << to_string(nt);
    return os;
}

} // namespace whilelang
