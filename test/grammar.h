#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

// Structure representing a production rule.
struct Production {
    string lhs;
    vector<string> rhs;
};

// The symbol for epsilon (empty production).
extern const string EPSILON;

// Inline helper to check if a symbol is a nonterminal (enclosed in angle brackets).
inline bool isNonTerminal(const string &s) {
    return !s.empty() && s.front() == '<' && s.back() == '>';
}

// Function prototypes.
vector<Production> buildGrammar();
map<string, set<string>> computeFirstSets(const vector<Production>& grammar);
map<string, set<string>> computeFollowSets(const vector<Production>& grammar, const map<string, set<string>> &first);
map<pair<string, string>, vector<int>> constructParsingTable(const vector<Production>& grammar,
                                                              const map<string, set<string>> &first,
                                                              const map<string, set<string>> &follow);

#endif // GRAMMAR_H
