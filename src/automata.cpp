#include <cstdlib>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>

using nonce_t = std::pair<int64_t, int64_t>;

static std::random_device rd;
static std::uniform_int_distribution<int64_t> U(0, ~0);
nonce_t nonce() {
    std::mt19937 gen(rd());
    return std::make_pair(U(gen), U(gen));
}

namespace std {
template <> struct hash<nonce_t> {
    size_t operator()(const nonce_t& nonce) const {
        return std::hash<int64_t>()(nonce.first) ^ std::hash<int64_t>()(nonce.second);
    }
};
} // namespace std

template <typename Label> class NFA {
  public:
    NFA(const NFA& other)
        : start(other.start), transitions(other.transitions), accepting(other.accepting),
          alphabet(other.alphabet) {}

    NFA(const char* string) : start(nonce()) {
        auto state = start;
        for (int i = 0; string[i] != '\0'; ++i) {
            char c = string[i];
            auto next = nonce();
            transitions[state] = {{c, {next}}};
            state = next;
            alphabet.insert(c);
        }
        accepting[state] = Label{};
    }

    NFA operator|(const NFA& other) {
        // Combine transitions
        NFA result{*this};
        result.transitions.insert(other.transitions.begin(), other.transitions.end());
        // Combine accepting states
        result.accepting.insert(other.accepting.begin(), other.accepting.end());
        // Link the start states
        auto new_start = nonce();
        result.transitions[new_start] = {{'\0', {start, other.start}}};
        result.start = new_start;
        result.alphabet = other.alphabet;
        result.alphabet.insert(alphabet.begin(), alphabet.end());
        return result;
    }

    NFA operator+(const NFA& other) {
        NFA result;
        // Combine transitions
        result.transitions = other.transitions;
        result.transitions.insert(transitions.begin(), transitions.end());
        // Link LHS accepting to RHS start state
        for (auto& accept : accepting) {
            result.transitions[accept.first] = {{'\0', {other.start}}};
        }
        // Use LHS start state
        result.start = start;
        // And RHS accepting states
        result.accepting = other.accepting;
        return result;
    }

    void to_dfa() {
        std::map<std::set<nonce_t>, nonce_t> subset_map;
        Adj dfa;
        std::queue<std::set<nonce_t>> q;
        std::set<nonce_t> powerset = {start};
        q.push(powerset);
        while (!q.empty()) {
            auto subset = q.front();
            q.pop();
            for (char c : alphabet) {
                std::set<nonce_t> succ;
                for (auto [state, edges] : transitions) {
                    if (subset.count(state) && edges.count(c)) {
                        auto& next = edges[c];
                        succ.insert(next.begin(), next.end());
                    }
                }
                if (succ.empty())
                    continue;
                if (!dfa.count(succ)) {
                    dfa[succ] = {};
                    q.push(succ);
                }
                dfa[subset][c].insert(succ);
            }
        }
        transitions.clear();
        for (auto& [subset, edges] : dfa) {
            transitions[nonce()] = edges;
        }
        accepting.clear();
    }

  private:
    using Adj = std::unordered_map<nonce_t, std::unordered_map<char, std::set<nonce_t>>>;
    nonce_t start;
    Adj transitions;
    std::unordered_map<nonce_t, Label> accepting;
    std::set<char> alphabet;
};
