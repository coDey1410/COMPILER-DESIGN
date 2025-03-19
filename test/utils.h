#ifndef UTILS_H
#define UTILS_H

#include "grammar.h"
#include <string>
#include <map>
#include <set>
using namespace std;

// Prototypes for utility functions.
void generateCFGGrammarFile(const vector<Production>& grammar, const string &filename);
void writeParsingInfoToFile(const vector<Production>& grammar,
                            const map<string, set<string>> &first,
                            const map<string, set<string>> &follow,
                            const map<pair<string, string>, vector<int>> &parsingTable);

#endif // UTILS_H
