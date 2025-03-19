#include "parser.h"
#include <iostream>
using namespace std;

void ll1Parse(const vector<Token>& tokens, const vector<Production>& grammar,
              const map<pair<string, string>, vector<int>> &parsingTable) {
    // Initialize a stack with end-marker "$" and the start symbol.
    vector<string> stack;
    stack.push_back("$");
    stack.push_back("<sql>");
    
    int i = 0;  // Token stream index.
    
    while (!stack.empty()) {
        string top = stack.back();
        // Map current token to the grammar's terminal representation.
        string inputSymbol;
        if (tokens[i].type == TokenType::END)
            inputSymbol = "$";
        else {
            switch (tokens[i].type) {
                case TokenType::IDENTIFIER: inputSymbol = "id"; break;
                case TokenType::NUMBER: inputSymbol = "num"; break;
                case TokenType::STRING: inputSymbol = "str"; break;
                default: inputSymbol = tokens[i].lexeme; break;
            }
        }
        
        // If top of stack is a terminal or the end marker.
        if (top == "$" || !isNonTerminal(top)) {
            if (top == inputSymbol) {
                stack.pop_back();
                i++;
            } else {
                cerr << "Syntax Error: expected '" << top << "' but found '" << inputSymbol << "'" << endl;
                return;
            }
        } else {
            // Top is a nonterminal; look up the table.
            pair<string, string> key = {top, inputSymbol};
            if (parsingTable.find(key) == parsingTable.end()) {
                cerr << "Syntax Error: no rule for (" << top << ", " << inputSymbol << ")" << endl;
                return;
            }
            vector<int> prodIndices = parsingTable.at(key);
            if (prodIndices.size() != 1) {
                cerr << "Syntax Error: conflict in parsing table for (" << top << ", " << inputSymbol << ")" << endl;
                return;
            }
            int prodIndex = prodIndices[0];
            Production prod = grammar[prodIndex];
            stack.pop_back();
            if (!(prod.rhs.size() == 1 && prod.rhs[0] == EPSILON)) {
                // Push right-hand side in reverse order.
                for (int j = prod.rhs.size() - 1; j >= 0; j--) {
                    stack.push_back(prod.rhs[j]);
                }
            }
        }
    }
    
    if (i != tokens.size()) {
        cerr << "Syntax Error: input not fully consumed." << endl;
        return;
    }
    cout << "Parsing successful: Input is syntactically correct according to the LL(1) grammar." << endl;
}
