#include "parser.hpp"
#include "whilelang.hpp"
#include <vector>

namespace parser {

    void ParserGenerator::index_grammar(){
        for (Production p : source_grammar) {
            by_lhs[p.lhs].push_back(p.rhs);
        }
    }
    
    // First over a single nonterminal
    void ParserGenerator::compute_first_sets(){
        auto cmp = &Symbol::less;

        for (auto& kv : by_lhs) {
            FIRST_NT.emplace(kv.first, std::set<Symbol, decltype(cmp)>(cmp));
        }

        bool changed = true;

        while(changed){
            changed = false;
            
            for (auto const& [A, rhss] : by_lhs){
                auto& FA = FIRST_NT[A];
                for(auto const& rhs : rhss) {
                    auto first_rhs = first_of_seq(rhs);
                    for(Symbol const& s : first_rhs) {
                        if(s.is_epsilon()){
                            if (FA.insert(EPS).second)
                                changed = true;
                        }
                        else{
                            if (FA.insert(s).second)
                                changed = true;
                        }
                    }
                }
            }
        }
    }
    
    
    std::set<Symbol, decltype(&Symbol::less)> ParserGenerator::first_of_seq(std::vector<Symbol> const& seq) const {
        auto cmp = &Symbol::less;
        std::set<Symbol, decltype(cmp)> out(cmp);
        bool all_nullable = true;

        for (size_t i = 0; i < seq.size(); i++){
            const Symbol& X = seq[i];

            if(X.is_terminal()){
                out.insert(X);
                all_nullable = false;
                break;
            }
            
            // Continue on reading the language
            if(X.is_epsilon())
                continue;
            
            // The meat and potatoes
            if(X.is_nonterm()){
                auto it = FIRST_NT.find(static_cast<whilelang::Nonterm>(X.id));

                // nonterm w/ no productions
                if (it == FIRST_NT.end()){
                    all_nullable = false;
                    break;
                }

                // FIRST(X) \ { EPS }
                for (auto const& s : it->second) {
                    if (!s.is_epsilon())
                        out.insert(s);
                }
                
                // FIRST(X) does nto contain epsilon
                if (it->second.find(EPS) == it->second.end()){
                    all_nullable = false;
                    break;
                }

                // continue to next symbol, everythings nullable at this point
                continue;
            }
        }
        if (seq.empty() || all_nullable){
            out.insert(EPS);
        }
        
        return out;
    }
    
    // Concat gamma tail with singele lookahead
    std::set<Symbol, decltype(&Symbol::less)> ParserGenerator::gamma_then_a(std::vector<Symbol> const& gamma_tail, Symbol aSym) const {

        std::vector<Symbol> seq = gamma_tail;

        seq.push_back(aSym);
        return first_of_seq(seq);
    }


    std::set<LR1Item, LR1ItemLess> ParserGenerator::closure(std::set<LR1Item, LR1ItemLess> I) const {
        std::set<LR1Item, LR1ItemLess> C = I;
        
        bool changed = true;
        while(changed) {
            changed = false;

            for(auto itItem = C.begin(); itItem != C.end(); ++itItem){
                const LR1Item& item = *itItem;

                // Gamma not empty and first is nonterminal
                if(item.gamma.empty()) continue;

                const Symbol& first = item.gamma.front();

                if(!first.is_nonterm()) continue;

                whilelang::Nonterm B = static_cast<whilelang::Nonterm>(first.id);

                // Compute FIRST where gamma' is tail after B
            
                std::vector<Symbol>gamma_tail(item.gamma.begin() + 1, item.gamma.end());
                auto LA = gamma_then_a(gamma_tail, item.a);
            
                // Expand on every production B -> delta
                
                auto prodIt = by_lhs.find(B);

                if (prodIt == by_lhs.end()) continue; 

                for (auto const& delta : prodIt->second) {
                    for (Symbol const& b : LA) {
                        if(b.is_epsilon()) continue; // The diagnosis (lookahead) is terminal 
                                                        // or we're at the end, yay!
                        
                        // fresh stuff to add to the table
                        LR1Item newItem{ItemType::PARTIAL, B, {}, delta, b}; 

                        auto [_, inserted] = C.insert(std::move(newItem));
                        
                        // Only consider changed if we're actually adding something new and not just revisiting a new item we've seen.
                        if(inserted) {
                            changed = true;
                        }
                    }
                }
            }
        }

        return C;
    }

    
    std::set<LR1Item, LR1ItemLess> ParserGenerator::goto_set(std::set<LR1Item, LR1ItemLess> const& I, Symbol X) const {
        std::set<LR1Item, LR1ItemLess> J;

        for(const LR1Item& item : I) {
            if(!item.gamma.empty() && item.gamma.front() == X){
                LR1Item newItem = item;

                newItem.alpha.push_back(item.gamma.front());
                newItem.gamma.erase(newItem.gamma.begin());

                J.insert(newItem);
            }
        }
    }
}
