#pragma once

#include <set>
#include <vector>

#include "whilelang.hpp"
#include "token.hpp"


namespace parser {

    enum class SymKind : uint8_t {Terminal, NonTerminal, Epsilon, End }; 

    struct Symbol {
        SymKind kind;
        uint16_t id;
    };

    inline Symbol T(TokenType t)   { return {SymKind::Terminal,    static_cast<uint16_t>(t)}; }
    inline Symbol N(whilelang::Nonterm nt)    { return {SymKind::NonTerminal, static_cast<uint16_t>(nt)}; }
    constexpr Symbol EPS{SymKind::Epsilon, 0};
    constexpr Symbol END{SymKind::End, 0};

    struct Production {
        whilelang::Nonterm lhs;
        std::vector<Symbol> rhs;
    };

    enum ItemType {
        POSSIBILITY = 0,
        PARTIAL = 1,
        COMPLETE = 2
    };

    struct LR1Item {
        ItemType it;
        whilelang::Nonterm alpha;          // nonterminal which can be used for production
        std::vector<Symbol> beta;   //grammar sequence we've seen
        std::vector<Symbol> gamma;  //grammar sequence we haven't seen
        Symbol a; // lookahead
    };

    class ParserGenerator{
        public:

        ParserGenerator(std::initializer_list<Production> prods) : source_grammar(prods) {}

        private:
            std::vector<Production> source_grammar;

            std::set<LR1Item> closure(std::set<LR1Item>& s);
    };
}
