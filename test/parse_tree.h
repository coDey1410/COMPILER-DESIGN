// // parse_tree.h
// #ifndef PARSE_TREE_H
// #define PARSE_TREE_H

// #include <string>
// #include <vector>
// #include <memory>
// #include <fstream>
// #include <iostream>
// #include "token.h"
// #include "grammar.h"

// using namespace std;

// // Forward declaration of isNonTerminal in case it is not defined in grammar.h.
// // (Assumes nonterminals are enclosed within '<' and '>')


// // Node in the parse tree
// class ParseTreeNode {
// public:
//     string symbol;   // Grammar symbol (terminal or non-terminal)
//     string lexeme;   // Actual text (only non-empty for terminals)
//     vector<shared_ptr<ParseTreeNode>> children;  // Child nodes

//     ParseTreeNode(const string &sym) : symbol(sym), lexeme("") {}
//     ParseTreeNode(const string &sym, const string &lex) : symbol(sym), lexeme(lex) {}

//     // A node is terminal if its symbol is not a nonterminal (per our definition).
//     bool isTerminal() const {
//         return !isNonTerminal(symbol);
//     }
// };

// // Parse tree visualizer that writes the tree to a file
// class ParseTreeVisualizer {
// public:
//     static void printTreeToFile(const shared_ptr<ParseTreeNode> &node, const string &filename) {
//         ofstream outFile(filename);
//         if (!outFile) {
//             cerr << "Error: Unable to open file " << filename << " for writing." << endl;
//             return;
//         }

//         outFile << "=== LL(1) Parse Tree ===" << endl << endl;
//         printNode(outFile, node, 0);
//         outFile << endl << "=====================" << endl;
//         outFile.close();

//         cout << "Parse tree has been written to " << filename << endl;
//     }

// private:
//     static void printNode(ofstream &out, const shared_ptr<ParseTreeNode> &node, int depth) {
//         string indent(depth * 2, ' ');
//         if (node->isTerminal()) {
//             // For terminals, print the symbol and the lexeme (if available)
//             if (!node->lexeme.empty()) {
//                 out << indent << node->symbol << " [\"" << node->lexeme << "\"]" << endl;
//             } else {
//                 out << indent << node->symbol << endl;
//             }
//         } else {
//             out << indent << node->symbol << endl;
//         }

//         // Recursively print all child nodes at the increased depth
//         for (const auto &child : node->children) {
//             printNode(out, child, depth + 1);
//         }
//     }
// };

// // LL(1) parser that builds a parse tree and returns the root node.
// // It uses two stacks: one for grammar symbols (symbolStack) and one for the corresponding parse tree nodes (nodeStack).
// shared_ptr<ParseTreeNode> ll1ParseWithTree(
//     const vector<Token> &tokens,
//     const vector<Production> &grammar,
//     const map<pair<string, string>, vector<int>> &parsingTable
// ) {
//     // The symbol stack for LL(1) parsing.
//     vector<string> symbolStack;
//     // The node stack to maintain pointers to corresponding parse tree nodes.
//     vector<shared_ptr<ParseTreeNode>> nodeStack;

//     // Create the root node (<sql>) and push it along with the end-marker.
//     auto rootNode = make_shared<ParseTreeNode>("<sql>");
//     symbolStack.push_back("$");
//     symbolStack.push_back("<sql>");
//     nodeStack.push_back(rootNode);

//     int i = 0;  // Index into the token stream.

//     // Main LL(1) parsing loop.
//     while (!symbolStack.empty() ) {
//         // Ensure we have not run out of tokens.
//         if (i >= tokens.size()) {
//             cerr << "Error: Attempting to access token beyond array bounds" << endl;
//             return nullptr;
//         }

//         string top = symbolStack.back();
//         auto topNode = nodeStack.back();

//         // Map the current token to the grammar's terminal representation.
//         string inputSymbol;
//         if (tokens[i].type == TokenType::END)
//             inputSymbol = "$";
//         else {
//             switch (tokens[i].type) {
//                 case TokenType::IDENTIFIER: 
//                     inputSymbol = "id"; 
//                     break;
//                 case TokenType::NUMBER: 
//                     inputSymbol = "num"; 
//                     break;
//                 case TokenType::STRING: 
//                     inputSymbol = "str"; 
//                     break;
//                 default: 
//                     inputSymbol = tokens[i].lexeme; 
//                     break;
//             }
//         }
//         string inputLexeme = tokens[i].lexeme;

//         // If top of stack is a terminal (or end marker), match it with the input.
//         if (top == "$" || !isNonTerminal(top)) {
//             if (top == inputSymbol) {
//                 // For terminal nodes, record the lexeme.
//                 if (topNode) {
//                     topNode->lexeme = inputLexeme;
//                 }
//                 // Pop the terminal and move to the next token.
//                 symbolStack.pop_back();
//                 nodeStack.pop_back();
//                 i++;
//             } else {
//                 cerr << "Syntax Error: expected '" << top << "' but found '" << inputSymbol << "'" << endl;
//                 return nullptr;
//             }
//         } else {
//             // Top is a nonterminal: look up the corresponding production using the parsing table.
//             pair<string, string> key = {top, inputSymbol};
//             if (parsingTable.find(key) == parsingTable.end()) {
//                 cerr << "Syntax Error: no rule for (" << top << ", " << inputSymbol << ")" << endl;
//                 return nullptr;
//             }

//             vector<int> prodIndices = parsingTable.at(key);
//             if (prodIndices.size() != 1) {
//                 cerr << "Syntax Error: conflict in parsing table for (" << top << ", " << inputSymbol << ")" << endl;
//                 return nullptr;
//             }

//             int prodIndex = prodIndices[0];
//             Production prod = grammar[prodIndex];

//             // Pop the nonterminal.
//             symbolStack.pop_back();
//             nodeStack.pop_back();

//             // If the production is not an epsilon production, process its right-hand side.
//             if (!(prod.rhs.size() == 1 && prod.rhs[0] == EPSILON)) {
//                 // Temporary container to hold the new children in correct left-to-right order.
//                 vector<shared_ptr<ParseTreeNode>> newChildren;
//                 // Push the production symbols in reverse order so that the leftmost symbol is processed first.
//                 for (int j = prod.rhs.size() - 1; j >= 0; j--) {
//                     auto childNode = make_shared<ParseTreeNode>(prod.rhs[j]);
//                     // Insert at the beginning to preserve the order of children.
//                     newChildren.insert(newChildren.begin(), childNode);
//                     // Push onto the stacks.
//                     symbolStack.push_back(prod.rhs[j]);
//                     nodeStack.push_back(childNode);
//                 }
//                 // Attach the new children to the current parse tree node.
//                 for (const auto &child : newChildren) {
//                     topNode->children.push_back(child);
//                 }
//             } else {
//                 // For an epsilon production, add a node with the epsilon symbol (using "ε" for display).
//                 auto epsilonNode = make_shared<ParseTreeNode>("ε");
//                 topNode->children.push_back(epsilonNode);
//             }
//         }
//     }

//     if (i != tokens.size()) {
//         cerr << "Syntax Error: input not fully consumed." << endl;
//         return nullptr;
//     }

//     cout << "Parsing successful: Input is syntactically correct according to the LL(1) grammar." << endl;
//     return rootNode;
// }

// #endif // PARSE_TREE_H
