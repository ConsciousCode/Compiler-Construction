#include "parser.hpp"
#include "token.hpp"
#include "whilelang.hpp"

using namespace whilelang;

using P = Production;

namespace whilelang {
    class WhileParser {
        public:
            static void generate() {
                parser::ParserGenerator pg = parser::ParserGenerator{
                    // <prog> ::= <block>
                    P{Nonterm::Program, {N(Nonterm::Block)}},

                    // <block> ::= <block> ; <cmd> | <cmd>
                    P{Nonterm::Block, {
                        N(Nonterm::Block), T(TokenType::OP_SEMI), N(Nonterm::Cmd)
                    }},
                    P{Nonterm::Block, {N(Nonterm::Cmd)}},

                    // <cmd> ::= <ident> := <expr>
                    P{Nonterm::Cmd, {
                        T(TokenType::IDENT),
                        T(TokenType::OP_ASSIGN),
                        N(Nonterm::ArithExpr)
                    }},
                    // <cmd> ::= skip
                    P{Nonterm::Cmd, { T(TokenType::KW_SKIP) }},
                    // <cmd> ::= if <cond> then <block> else <block> fi
                    P{Nonterm::Cmd, {
                        T(TokenType::KW_IF), N(Nonterm::Cond),
                        T(TokenType::KW_THEN), N(Nonterm::Block),
                        T(TokenType::KW_ELSE), N(Nonterm::Block),
                        T(TokenType::KW_FI)
                    }},
                    // <cmd> ::= while <cond> do <block> od
                    P{Nonterm::Cmd, {
                        T(TokenType::KW_WHILE), N(Nonterm::Cond),
                        T(TokenType::KW_DO), N(Nonterm::Block),
                        T(TokenType::KW_OD)
                    }},
                    
                    // Arithmetic Operators
                    // <additiveop> ::= + | -
                    P{Nonterm::AdditiveOp, {T(TokenType::OP_ADD)}},
                    P{Nonterm::AdditiveOp, {T(TokenType::OP_SUB)}},

                    // Relational Operators
                    // <relop> ::= = | < | <= | > | >=
                    P{Nonterm::RelOp, {T(TokenType::OP_EQ)}},
                    P{Nonterm::RelOp, {T(TokenType::OP_LT)}},
                    P{Nonterm::RelOp, {T(TokenType::OP_LE)}},
                    P{Nonterm::RelOp, {T(TokenType::OP_GT)}},
                    P{Nonterm::RelOp, {T(TokenType::OP_GE)}},

                    // Arithmetic Expression
                    // <arithexpr> ::= <ident> | <integer> | <arithexpr> <relop> <arithexpr> | ( <arithexpr> )
                    P{Nonterm::ArithExpr, {N(Nonterm::Term)}},
                    P{Nonterm::ArithExpr, {
                        N(Nonterm::ArithExpr),
                        N(Nonterm::AdditiveOp),
                        N(Nonterm::Term)
                    }},
                    P{Nonterm::Term, {N(Nonterm::Factor)}},
                    P{Nonterm::Term, {
                        N(Nonterm::Term),
                        T(TokenType::OP_MUL),
                        N(Nonterm::Factor)
                    }},
                    P{Nonterm::Factor, {T(TokenType::IDENT)}},
                    P{Nonterm::Factor, {T(TokenType::INTEGER)}},
                    P{Nonterm::Factor, {
                        T(TokenType::OP_LPAREN),
                        N(Nonterm::ArithExpr),
                        T(TokenType::OP_RPAREN)
                    }},
                    
                    // <cond> ::= <disjunction>
                    P{Nonterm::Cond, { N(Nonterm::Disjunction) }},
                    // <disjunction> ::= <conjunction>
                    P{Nonterm::Disjunction, { N(Nonterm::Conjunction) }},
                    // <disjunction> ::= <disjunction> or <conjunction>
                    P{Nonterm::Disjunction, {
                        N(Nonterm::Disjunction),
                        T(TokenType::OP_OR),
                        N(Nonterm::Conjunction)
                    }},
                    // <conjunction> ::= <bool_expr>
                    P{Nonterm::Conjunction, { N(Nonterm::BoolExpr) }},
                    // <conjunction> ::= <conjunction> and <bool_expr>
                    P{Nonterm::Conjunction, {
                        N(Nonterm::Conjunction),
                        T(TokenType::OP_AND),
                        N(Nonterm::BoolExpr)
                    }},

                    // Boolean Expression
                    // <boolexpr> ::= true | false | not [ <boolexpr> ] | [ <boolexpr> ]
                    P{Nonterm::BoolExpr, {T(TokenType::KW_TRUE)}},
                    P{Nonterm::BoolExpr, {T(TokenType::KW_FALSE)}},
                    P{Nonterm::BoolExpr, {
                        T(TokenType::OP_NOT),
                        T(TokenType::OP_LBRACK),
                        N(Nonterm::BoolExpr),
                        T(TokenType::OP_RBRACK)
                    }},
                    P{Nonterm::BoolExpr, {
                        T(TokenType::OP_LBRACK),
                        N(Nonterm::BoolExpr),
                        T(TokenType::OP_RBRACK)
                    }},
                    // <boolexpr> ::= <arithexpr> <relop> <arithexpr>
                    P{Nonterm::BoolExpr, {
                        N(Nonterm::ArithExpr),
                        N(Nonterm::RelOp),
                        N(Nonterm::ArithExpr)
                    }},
                    
                };
            }
    };
}
