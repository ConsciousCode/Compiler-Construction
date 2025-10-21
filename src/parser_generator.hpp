#pragma once

#include <set>
#include <vector>

#include "while_symbols.hpp"

using namespace while_symbols;

namespace parser_generator {

    enum ItemType {
        POSSIBILITY = 0,
        PARTIAL = 1,
        COMPLETE = 2
    };

    struct LR1Item {
        ItemType it;
        Nonterm alpha;          // nonterminal which can be used for production
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
