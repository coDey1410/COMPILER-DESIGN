#ifndef PARSER_H
#define PARSER_H

#include "grammar.h"
#include "lexer.h"
#include "token.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

// Prototype for LL(1) parser function that uses the parsing table.
void ll1Parse(const vector<Token>& tokens, const vector<Production>& grammar,
              const map<pair<string, string>, vector<int>> &parsingTable);

#endif // PARSER_H
