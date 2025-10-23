#include "parser.hpp"
#include "token.hpp"
#include "whilelang.hpp"

#include <iostream>
#include <queue>
#include <vector>

namespace parser {

void ParserGenerator::index_grammar() {
    for (Production p : source_grammar) {
        by_lhs[p.lhs].push_back(p.rhs);
    }
}

// First over a single nonterminal
void ParserGenerator::compute_first_sets() {
    auto cmp = &Symbol::less;

    for (auto& kv : by_lhs) {
        FIRST_NT.emplace(kv.first, std::set<Symbol, decltype(cmp)>(cmp));
    }

    bool changed = true;

    while (changed) {
        changed = false;

        for (auto const& [A, rhss] : by_lhs) {
            auto& FA = FIRST_NT[A];
            for (auto const& rhs : rhss) {
                auto first_rhs = first_of_seq(rhs);
                for (Symbol const& s : first_rhs) {
                    if (s.is_epsilon()) {
                        if (FA.insert(EPS).second)
                            changed = true;
                    } else {
                        if (FA.insert(s).second)
                            changed = true;
                    }
                }
            }
        }
    }
}

std::set<Symbol, decltype(&Symbol::less)>
ParserGenerator::first_of_seq(std::vector<Symbol> const& seq) const {
    auto cmp = &Symbol::less;
    std::set<Symbol, decltype(cmp)> out(cmp);
    bool all_nullable = true;

    for (size_t i = 0; i < seq.size(); i++) {
        const Symbol& X = seq[i];

        if (X.kind == SymKind::End) {
            out.insert(X);
            all_nullable = false;
            break;
        }

        if (X.is_terminal()) {
            out.insert(X);
            all_nullable = false;
            break;
        }

        // Continue on reading the language
        if (X.is_epsilon())
            continue;

        // The meat and potatoes
        if (X.is_nonterm()) {
            auto it = FIRST_NT.find(static_cast<whilelang::Nonterm>(X.id));

            // nonterm w/ no productions
            if (it == FIRST_NT.end()) {
                all_nullable = false;
                break;
            }

            // FIRST(X) \ { EPS }
            for (auto const& s : it->second) {
                if (!s.is_epsilon())
                    out.insert(s);
            }

            // FIRST(X) does nto contain epsilon
            if (it->second.find(EPS) == it->second.end()) {
                all_nullable = false;
                break;
            }

            // continue to next symbol, everythings nullable at this point
            continue;
        }
    }
    if (seq.empty() || all_nullable) {
        out.insert(EPS);
    }

    return out;
}

// Concat gamma tail with singele lookahead
std::set<Symbol, decltype(&Symbol::less)>
ParserGenerator::gamma_then_a(std::vector<Symbol> const& gamma_tail, Symbol aSym) const {

    std::vector<Symbol> seq = gamma_tail;

    seq.push_back(aSym);
    return first_of_seq(seq);
}

std::set<LR1Item, LR1ItemLess> ParserGenerator::closure(std::set<LR1Item, LR1ItemLess> I) const {
    std::set<LR1Item, LR1ItemLess> C = I;

    bool changed = true;
    while (changed) {
        changed = false;

        for (auto itItem = C.begin(); itItem != C.end(); ++itItem) {
            const LR1Item& item = *itItem;

            // Gamma not empty and first is nonterminal
            if (item.gamma.empty())
                continue;

            const Symbol& first = item.gamma.front();

            if (!first.is_nonterm())
                continue;

            whilelang::Nonterm B = static_cast<whilelang::Nonterm>(first.id);

            // Compute FIRST where gamma' is tail after B

            std::vector<Symbol> gamma_tail(item.gamma.begin() + 1, item.gamma.end());
            auto LA = gamma_then_a(gamma_tail, item.a);

            // Expand on every production B -> delta

            auto prodIt = by_lhs.find(B);

            if (prodIt == by_lhs.end())
                continue;

            for (auto const& delta : prodIt->second) {
                for (Symbol const& b : LA) {
                    if (b.is_epsilon())
                        continue; // The diagnosis (lookahead) is terminal
                                  // or we're at the end, yay!

                    // fresh stuff to add to the table
                    LR1Item newItem{ItemType::PARTIAL, B, {}, delta, b};

                    auto [_, inserted] = C.insert(std::move(newItem));

                    // Only consider changed if we're actually adding something new and not just
                    // revisiting a new item we've seen.
                    if (inserted) {
                        changed = true;
                    }
                }
            }
        }
    }

    return C;
}

std::set<LR1Item, LR1ItemLess> ParserGenerator::goto_set(std::set<LR1Item, LR1ItemLess> const& I,
                                                         Symbol X) const {
    std::set<LR1Item, LR1ItemLess> J;

    for (const LR1Item& item : I) {
        if (!item.gamma.empty() && item.gamma.front() == X) {
            LR1Item newItem;
            newItem.it = item.it;
            newItem.alpha = item.alpha;
            newItem.a = item.a;

            // Build beta: old beta + X
            newItem.beta = item.beta;
            newItem.beta.push_back(X);

            // Build gamma: old gamma without first element
            newItem.gamma = std::vector<Symbol>(item.gamma.begin() + 1, item.gamma.end());

            J.insert(newItem);
        }
    }

    return closure(J);
}

std::set<Symbol> ParserGenerator::all_symbols() const {
    std::set<Symbol> symbols;

    // Terminals
    for (int i = 0; i < static_cast<int>(TokenType::COUNT); i++) {
        symbols.insert(T(static_cast<TokenType>(i)));
    }

    // Nonterminals
    for (const auto& [nt, _] : by_lhs) {
        symbols.insert(N(nt));
    }

    return symbols;
}

std::set<LR1Item, LR1ItemLess> ParserGenerator::initial_items() const {
    std::set<LR1Item, LR1ItemLess> I0;

    whilelang::Nonterm start = whilelang::Nonterm::Program;

    auto it = by_lhs.find(start);

    if (it != by_lhs.end()) {
        for (const auto& rhs : it->second) {
            LR1Item item{ItemType::PARTIAL, start, {}, rhs, END};
            I0.insert(item);
        }
    }

    return closure(I0);
}

size_t ParserGenerator::find_or_add_state(std::set<LR1Item, LR1ItemLess> const& items) {
    for (size_t i = 0; i < states.size(); i++) {
        if (states[i] == items) {
            return i;
        }
    }

    states.push_back(items);
    return states.size() - 1;
}

void ParserGenerator::build_automaton() {
    index_grammar();
    compute_first_sets();

    // Initial set
    auto I0 = initial_items();
    size_t start_state = find_or_add_state(I0);

    // Worklist
    std::queue<size_t> worklist;
    worklist.push(start_state);
    std::set<size_t> visited;

    // Build the parsing table
    while (!worklist.empty()) {
        size_t state_id = worklist.front();

        worklist.pop();

        if (visited.count(state_id))
            continue;
        visited.insert(state_id);

        auto I = states[state_id];

        for (const Symbol& X : all_symbols()) {
            auto J = goto_set(I, X);

            if (!J.empty()) {
                size_t next_state = find_or_add_state(J);
                transitions[{state_id, X}] = next_state;

                if (!visited.count(next_state)) {
                    worklist.push(next_state);
                }
            }
        }
    }
}

size_t ParserGenerator::production_index(whilelang::Nonterm lhs,
                                         std::vector<Symbol> const& rhs) const {
    for (size_t i = 0; i < source_grammar.size(); i++) {
        if (source_grammar[i].lhs == lhs && source_grammar[i].rhs == rhs) {
            return i;
        }
    }

    std::cerr << "ERROR: Production not found" << std::endl;
    std::abort();
}

void ParserGenerator::build_parse_tables() {
    for (size_t state_id = 0; state_id < states.size(); state_id++) {
        const auto& items = states[state_id];

        for (const LR1Item& item : items) {
            // case 1: shift
            if (!item.gamma.empty() && item.gamma.front().is_terminal()) {
                Symbol a = item.gamma.front();

                auto trans_it = transitions.find({state_id, a});

                if (trans_it != transitions.end()) {
                    size_t next_state = trans_it->second;

                    auto key = std::make_pair(state_id, a);

                    if (action_table.count(key)) {
                        std::cerr << "Conflict at state" << state_id << " on terminal " << a.id
                                  << std::endl;
                    }

                    action_table[key] = {ActionType::SHIFT, next_state};
                }
            }

            // Case 2: reduce

            else if (item.gamma.empty()) {
                bool is_accept = (item.alpha == whilelang::Nonterm::Program && item.a == END);

                if (is_accept) {
                    action_table[{state_id, END}] = {ActionType::ACCEPT, 0};
                } else {
                    size_t prod_id = production_index(item.alpha, item.beta);

                    auto key = std::make_pair(state_id, item.a);

                    if (action_table.count(key)) {
                        std::cerr << "Conflict at state " << state_id << " on lookahead "
                                  << item.a.id << std::endl;
                    }

                    action_table[key] = {ActionType::REDUCE, prod_id};
                }
            }
        }

        // Fill goto
        for (const auto& [trans_key, next_state] : transitions) {
            if (trans_key.first == state_id && trans_key.second.is_nonterm()) {
                goto_table[trans_key] = next_state;
            }
        }
    }
}

// Printing tables generated with the help of Anthropic Claude LLM
void ParserGenerator::print_action_table() const {
    std::cout << "ACTION Table:\n";
    for (const auto& [key, action] : action_table) {
        auto [state, symbol] = key;
        std::cout << "State " << state << ", Terminal " << symbol.id << " -> ";

        switch (action.type) {
        case ActionType::SHIFT:
            std::cout << "SHIFT " << action.value;
            break;
        case ActionType::REDUCE:
            std::cout << "REDUCE " << action.value;
            break;
        case ActionType::ACCEPT:
            std::cout << "ACCEPT";
            break;
        case ActionType::ERROR:
            std::cout << "ERROR";
            break;
        }
        std::cout << "\n";
    }
}

void ParserGenerator::print_goto_table() const {
    std::cout << "\nGOTO Table:\n";
    for (const auto& [key, next_state] : goto_table) {
        auto [state, symbol] = key;
        std::cout << "State " << state << ", Nonterminal " << symbol.id << " -> State "
                  << next_state << "\n";
    }
}

const Production& ParserGenerator::get_production(size_t idx) const {
    return source_grammar[idx];
}

// These nice operator overloads were generated by Claude
std::ostream& operator<<(std::ostream& os, const Symbol& sym) {
    if (sym.is_terminal()) {
        os << "T:" << sym.id; // Or map sym.id to token name
    } else {
        os << "N:" << sym.id; // Or map sym.id to nonterminal name
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const LR1Item& item) {
    // Print the nonterminal
    os << item.alpha << " -> ";

    // Print beta (what we've seen)
    for (const auto& sym : item.beta) {
        os << sym << " ";
    }

    // Print the dot
    os << "• ";

    // Print gamma (what we haven't seen)
    for (const auto& sym : item.gamma) {
        os << sym << " ";
    }

    // Print lookahead
    os << ", " << item.a;

    return os;
}

} // namespace parser
