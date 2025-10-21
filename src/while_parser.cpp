#include "parser_generator.hpp"
#include "token.hpp"
#include "while_symbols.hpp"

using namespace while_symbols;

using P = Production;

namespace while_parser {
    class WhileParser {
        public:
            static void generate() {
                parser_generator::ParserGenerator pg = parser_generator::ParserGenerator{
                    P{Nonterm::Program, {N(Nonterm::Block)}},

                    // Block of commands
                    P{Nonterm::Block, {N(Nonterm::Block), T(TokenType::OP_SEMI), N(Nonterm::Cmd)}},
                    P{Nonterm::Block, {N(Nonterm::Cmd)}},

                    // Commands
                    P{Nonterm::Cmd, { T(TokenType::IDENT), T(TokenType::OP_ASSIGN), N(Nonterm::Expr) }},
                    P{Nonterm::Cmd, { T(TokenType::KW_SKIP) }},
                    P{Nonterm::Cmd, { T(TokenType::KW_IF), N(Nonterm::Cond), T(TokenType::KW_THEN), N(Nonterm::Block), T(TokenType::KW_ELSE), N(Nonterm::Block), T(TokenType::KW_FI)}},
                    P{Nonterm::Cmd, { T(TokenType::KW_WHILE), N(Nonterm::Cond), T(TokenType::KW_DO), N(Nonterm::Block), T(TokenType::KW_OD)}},
                    
                    // Arithmetic Operators
                    P{Nonterm::ArithOp, {T(TokenType::OP_ADD)}}, 
                    P{Nonterm::ArithOp, {T(TokenType::OP_SUB)}}, 
                    P{Nonterm::ArithOp, {T(TokenType::OP_MUL)}}, 

                    // Relational Operators
                    P{Nonterm::RelOp, {T(TokenType::OP_EQ)}}, 
                    P{Nonterm::RelOp, {T(TokenType::OP_LT)}}, 
                    P{Nonterm::RelOp, {T(TokenType::OP_LE)}},
                    P{Nonterm::RelOp, {T(TokenType::OP_GT)}}, 
                    P{Nonterm::RelOp, {T(TokenType::OP_GE)}},

                    // Logical Operators
                    P{Nonterm::LogicalOp, {T(TokenType::OP_AND)}}, 
                    P{Nonterm::LogicalOp, {T(TokenType::OP_OR)}},

                    // Arithmetic Expression
                    P{Nonterm::ArithExpr, {T(TokenType::IDENT)}}, 
                    P{Nonterm::ArithExpr, {T(TokenType::INTEGER)}}, 
                    P{Nonterm::ArithExpr, {N(Nonterm::ArithExpr), N(Nonterm::RelOp), N(Nonterm::ArithExpr) }}, 
                    P{Nonterm::ArithExpr, {T(TokenType::OP_LPAREN), N(Nonterm::ArithExpr), T(TokenType::OP_LPAREN)}}, 

                    // Boolean Expression
                    P{Nonterm::BoolExpr, {T(TokenType::KW_TRUE)}}, 
                    P{Nonterm::BoolExpr, {T(TokenType::KW_FALSE)}}, 
                    P{Nonterm::BoolExpr, {T(TokenType::OP_LBRACK), N(Nonterm::BoolExpr), T(TokenType::OP_RBRACK)}}, 
                    P{Nonterm::BoolExpr, {N(Nonterm::BoolExpr), N(Nonterm::LogicalOp), N(Nonterm::BoolExpr)}}, 
                    P{Nonterm::BoolExpr, {N(Nonterm::ArithExpr), N(Nonterm::RelOp), N(Nonterm::ArithExpr)}}, 
                    
                };
            }
    };
}
