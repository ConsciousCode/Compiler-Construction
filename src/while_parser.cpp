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
                    P{Nonterm::Block, {N(Nonterm::CmdList)}}, // TODO: Reconsider design of command list vs `cmd ; block`

                    // Commands
                    P{Nonterm::Cmd, { T(TokenType::IDENT), T(TokenType::OP_ASSIGN), N(Nonterm::Expr) }}, // Assignment
                };
            }
    };
}
