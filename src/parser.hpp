#pragma once

#include <algorithm>
#include <memory>
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <string>

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
        
        inline bool operator<(const Symbol& other) const {
            return less(*this, other);
        }
        
        inline bool operator==(const Symbol& other) const {
            return kind == other.kind && id == other.id;
        }

        inline bool is_epsilon() const {
            return kind == SymKind::Epsilon;   
        }

        inline bool is_terminal() const {
            return kind == SymKind::Terminal;
        }
        
        inline bool is_nonterm() const {
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
        
        inline bool operator==(const LR1Item& other) const {
        return it == other.it &&
               alpha == other.alpha &&
               beta == other.beta &&
               gamma == other.gamma &&
               a == other.a;
    }
    };

    struct LR1ItemLess{
        bool operator()(LR1Item const& x, LR1Item const& y) const {
            if (x.it != y.it) return x.it < y.it;
            if (x.alpha != y.alpha) return x.alpha < y.alpha;
            
            if (x.beta != y.beta) {
                return std::lexicographical_compare(
                    x.beta.begin(), x.beta.end(),
                    y.beta.begin(), y.beta.end(),
                    Symbol::less
                );
            }
            
            if (x.gamma != y.gamma) {
                return std::lexicographical_compare(
                    x.gamma.begin(), x.gamma.end(),
                    y.gamma.begin(), y.gamma.end(),
                    Symbol::less
                );
            }
            
            return Symbol::less(x.a, y.a);
        }
    };

    // Parse table stuff
    enum class ActionType {
        SHIFT,
        REDUCE,
        ACCEPT,
        ERROR
    };

    struct Action {
        ActionType type; 
        size_t value;
    };

    class ParserGenerator{
        public:

        ParserGenerator(std::initializer_list<Production> prods) : source_grammar(prods) {
            build_automaton();
            build_parse_tables();
        }
        
        void build_automaton();
        void build_parse_tables();

        // table printing
        void print_action_table() const;
        void print_goto_table() const;

            const Production& get_production(size_t idx) const;
            
            friend class Parser;

        private:
            std::vector<Production> source_grammar;
            std::vector<std::set<LR1Item, LR1ItemLess>> states;

            std::map<std::pair<size_t, Symbol>, size_t> transitions;

            // B -> {rhs1, rhs2...}
            std::unordered_map<whilelang::Nonterm, std::vector<std::vector<Symbol>>> by_lhs;

            // First nonterminals
            std::unordered_map<whilelang::Nonterm, std::set<Symbol, decltype(&Symbol::less)>> FIRST_NT{};

            //parse tables
            std::map<std::pair<size_t, Symbol>, Action> action_table;
            std::map<std::pair<size_t, Symbol>, size_t> goto_table;

            
            size_t production_index(whilelang::Nonterm lhs, std::vector<Symbol> const& rhs) const;

            void index_grammar();
            void compute_first_sets();
            std::set<Symbol, decltype(&Symbol::less)> first_of_seq(std::vector<Symbol> const& seq) const;
            std::set<Symbol, decltype(&Symbol::less)> gamma_then_a(std::vector<Symbol> const& gamma_tail, Symbol aSym) const; 
            std::set<LR1Item, LR1ItemLess> closure(std::set<LR1Item, LR1ItemLess> I) const;
            std::set<LR1Item, LR1ItemLess> goto_set(std::set<LR1Item, LR1ItemLess> const& I, Symbol X) const;
            std::set<LR1Item, LR1ItemLess> initial_items() const;
            std::set<Symbol> all_symbols() const;
            size_t find_or_add_state(std::set<LR1Item, LR1ItemLess> const& items);

    };

    struct ParseTreeNode {
        whilelang::Nonterm nonterminal;
        TokenType terminal;
        bool is_terminal;
        std::string lexeme;
        std::vector<std::shared_ptr<ParseTreeNode>> children;

        static std::shared_ptr<ParseTreeNode> make_nonterminal(whilelang::Nonterm nt);

        static std::shared_ptr<ParseTreeNode> make_terminal(TokenType tt, std::string lex);

        static void print_tree(std::shared_ptr<ParseTreeNode>node, int indent = 0);
    };

    class Parser {
        const ParserGenerator& pg;
        
        public:
            Parser(const ParserGenerator& generator) : pg(generator) {}
            std::shared_ptr<ParseTreeNode> parse(std::vector<Token> const& tokens);
    
    };

    class DotGenerator {
        private:
            int node_counter;
            std::ostringstream dot;
            
            int generate_node(std::shared_ptr<ParseTreeNode> node);
            std::string escape_dot(const std::string& str);
            
        public:
            DotGenerator() : node_counter(0) {}
            std::string generate(std::shared_ptr<ParseTreeNode> root);
    };
        
    // Helper function to write directly to file
    void write_dot_file(std::shared_ptr<ParseTreeNode> root, const std::string& filename);
}
