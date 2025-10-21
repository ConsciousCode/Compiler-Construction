#pragma once

#include <algorithm>
#include <filesystem>
#include <set>
#include <vector>

#include "whilelang.hpp"
#include "token.hpp"


namespace parser {

    enum class SymKind : uint8_t {Terminal, NonTerminal, Epsilon, End }; 

    struct Symbol {
        SymKind kind;
        uint16_t id;
        
        inline static bool less(const Symbol& a, const Symbol& b){
            if(a.kind != b.kind) return a.kind < b.kind;
            else return a.id < b.id;
        }

        inline bool is_epsilon(){
            return kind == SymKind::Epsilon;   
        }

        inline bool is_terminal(){
            return kind == SymKind::Terminal;
        }
        
        inline bool is_nonterm(){
            return kind == SymKind::NonTerminal;
        }
    };

    inline Symbol T(TokenType t)   { return {SymKind::Terminal,    static_cast<uint16_t>(t)}; }
    inline Symbol N(whilelang::Nonterm nt)    { return {SymKind::NonTerminal, static_cast<uint16_t>(nt)}; }
    constexpr Symbol EPS{SymKind::Epsilon, 0};
    constexpr Symbol END{SymKind::End, 0};

    // Necessary type for std::set
    struct SymVecLess{
        bool operator()(const std::vector<Symbol>& a, const std::vector<Symbol>& b) const {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), Symbol::less);
        }
    };

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

    struct LR1ItemLess{
        bool operator()(LR1Item const& x, LR1Item const& y) const {
            if (x.it != y.it) return x.it < y.it;
            if (x.alpha != y.alpha) return x.alpha < y.alpha;
            // this next bit could probably be more efficient but its fine for now
            if (SymVecLess{}(x.beta, y.beta)) return true;
            if (SymVecLess{}(y.beta, x.beta)) return false;
            if (SymVecLess{}(x.gamma, y.gamma)) return true;
            if (SymVecLess{}(y.gamma, x.gamma)) return false;

            // if we haven't returned by now just check the lookahead
            return Symbol::less(x.a, y.a);
        }
    }; 

    class ParserGenerator{
        public:

        ParserGenerator(std::initializer_list<Production> prods) : source_grammar(prods) {}

        private:
            std::vector<Production> source_grammar;

            std::set<LR1Item> compute_closure(std::set<LR1Item>& s);
    };
}
