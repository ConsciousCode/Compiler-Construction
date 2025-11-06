#pragma once

#include "ast.hpp"
#include "token.hpp"
#include "whilelang.hpp"

#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace parser {

enum class SymKind : uint8_t { Terminal, NonTerminal, Epsilon, End };

struct Symbol {
    SymKind kind;
    uint16_t id;

    inline static bool less(const Symbol& a, const Symbol& b) {
        if (a.kind != b.kind)
            return a.kind < b.kind;
        else
            return a.id < b.id;
    }

    inline bool operator<(const Symbol& other) const {
        if (kind != other.kind)
            return kind < other.kind;
        else
            return id < other.id;
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

std::ostream& operator<<(std::ostream& os, const Symbol& sym);

inline Symbol T(TokenType t) {
    return {SymKind::Terminal, static_cast<uint16_t>(t)};
}
inline Symbol N(whilelang::Nonterm nt) {
    return {SymKind::NonTerminal, static_cast<uint16_t>(nt)};
}
constexpr Symbol EPS{SymKind::Epsilon, 0};
constexpr Symbol END{SymKind::End, 0};

// Necessary type for std::set
struct SymVecLess {
    bool operator()(const std::vector<Symbol>& a, const std::vector<Symbol>& b) const {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), Symbol::less);
    }
};

struct Production {
    whilelang::Nonterm lhs;
    std::vector<Symbol> rhs;
};

enum ItemType { POSSIBILITY = 0, PARTIAL = 1, COMPLETE = 2 };

struct LR1Item {
    ItemType it;
    whilelang::Nonterm alpha;  // nonterminal which can be used for production
    std::vector<Symbol> beta;  // grammar sequence we've seen
    std::vector<Symbol> gamma; // grammar sequence we haven't seen
    Symbol a;                  // lookahead

    inline bool operator==(const LR1Item& other) const {
        return it == other.it && alpha == other.alpha && beta == other.beta &&
               gamma == other.gamma && a == other.a;
    }
};

std::ostream& operator<<(std::ostream& os, const LR1Item& item);

struct LR1ItemLess {
    bool operator()(LR1Item const& x, LR1Item const& y) const {
        if (x.it != y.it)
            return x.it < y.it;
        if (x.alpha != y.alpha)
            return x.alpha < y.alpha;

        if (x.beta != y.beta) {
            return std::lexicographical_compare(x.beta.begin(), x.beta.end(), y.beta.begin(),
                                                y.beta.end(), Symbol::less);
        }

        if (x.gamma != y.gamma) {
            return std::lexicographical_compare(x.gamma.begin(), x.gamma.end(), y.gamma.begin(),
                                                y.gamma.end(), Symbol::less);
        }

        return Symbol::less(x.a, y.a);
    }
};

// Parse table stuff
enum class ActionType { SHIFT, REDUCE, ACCEPT, ERROR };

struct Action {
    ActionType type;
    size_t value;
};

class ParserGenerator {
  public:
    // Constructor: takes grammar productions and builds LR(1) parser tables
    ParserGenerator(std::initializer_list<Production> prods) : source_grammar(prods) {
        build_automaton();
        build_parse_tables();
    }

    // Build the LR(1) automaton (states and transitions)
    void build_automaton();
    
    // Populate ACTION and GOTO tables from the automaton
    void build_parse_tables();

    // Debug utilities for printing parse tables
    void print_action_table() const;
    void print_goto_table() const;

    // Retrieve a production rule by its index
    const Production& get_production(size_t idx) const;

    friend class Parser;

  private:
    // Original grammar productions
    std::vector<Production> source_grammar;
    
    // LR(1) automaton: collection of states (sets of LR(1) items)
    std::vector<std::set<LR1Item, LR1ItemLess>> states;

    // Transitions: maps (state, symbol) → next_state
    std::map<std::pair<size_t, Symbol>, size_t> transitions;

    // Grammar index: maps nonterminal → all its production RHS alternatives
    // e.g., Block → {[Cmd], [Block, ;, Cmd]}
    std::unordered_map<whilelang::Nonterm, std::vector<std::vector<Symbol>>> by_lhs;

    // FIRST sets: maps nonterminal → set of terminals that can start it
    std::unordered_map<whilelang::Nonterm, std::set<Symbol>> FIRST_NT{};

    // LR(1) parse tables
    std::map<std::pair<size_t, Symbol>, Action> action_table;  // (state, terminal) → action
    std::map<std::pair<size_t, Symbol>, size_t> goto_table;    // (state, nonterminal) → state

    // Find the index of a production in source_grammar
    size_t production_index(whilelang::Nonterm lhs, std::vector<Symbol> const& rhs) const;

    // Index grammar by LHS for faster lookup during closure computation
    void index_grammar();
    
    // Compute FIRST sets for all nonterminals
    void compute_first_sets();
    
    // Compute FIRST set for a sequence of symbols
    std::set<Symbol, decltype(&Symbol::less)> first_of_seq(std::vector<Symbol> const& seq) const;
    
    // Compute FIRST(gamma_tail · a) for lookahead calculation
    std::set<Symbol, decltype(&Symbol::less)> gamma_then_a(std::vector<Symbol> const& gamma_tail,
                                                           Symbol aSym) const;
    
    // Compute closure of a set of LR(1) items
    std::set<LR1Item, LR1ItemLess> closure(std::set<LR1Item, LR1ItemLess> I) const;
    
    // Compute GOTO(I, X): items reachable from I by shifting symbol X
    std::set<LR1Item, LR1ItemLess> goto_set(std::set<LR1Item, LR1ItemLess> const& I,
                                            Symbol X) const;
    
    // Generate initial LR(1) item set for start state
    std::set<LR1Item, LR1ItemLess> initial_items() const;
    
    // Get all symbols (terminals + nonterminals) in the grammar
    std::set<Symbol> all_symbols() const;
    
    // Find existing state with given items, or add as new state if not found
    size_t find_or_add_state(std::set<LR1Item, LR1ItemLess> const& items);
};

struct ParseTreeNode {
    whilelang::Nonterm nonterminal;
    TokenType terminal;
    bool is_terminal;
    std::shared_ptr<Token> token;
    std::vector<std::shared_ptr<ParseTreeNode>> children;

    static std::shared_ptr<ParseTreeNode> make_nonterminal(whilelang::Nonterm nt, Token t);

    static std::shared_ptr<ParseTreeNode> make_terminal(TokenType tt, Token t);

    static void print_tree(std::shared_ptr<ParseTreeNode> node, int indent = 0);

    TokenType cmd_type() const;
    ast::AST to_ast();
    ast::Cmd to_ast_cmd(std::vector<ast::ASTNode*>& labels);
    ast::Block to_ast_block(std::vector<ast::ASTNode*>& labels);
    ast::Expr to_ast_expr();
    ast::Cond to_ast_cond(std::vector<ast::ASTNode*>& labels);
    std::string get_token_value();
    ast::Arith::Op get_arith_op();
    ast::Comparison::Op get_comparison_op();
    ast::Logical::Op get_logical_op();
};

std::ostream& operator<<(std::ostream& os, const ParseTreeNode& node);

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
} // namespace parser
