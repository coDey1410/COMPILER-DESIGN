#include "utils.h"
#include <fstream>
#include <iostream>
#include "token.h"
using namespace std;

void generateCFGGrammarFile(const vector<Production>& grammar, const string &filename) {
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        return;
    }
    ofs << "=== CFG Grammar ===\n";
    for (const auto &prod : grammar) {
        ofs << prod.lhs << " ::= ";
        for (const auto &sym : prod.rhs)
            ofs << sym << " ";
        ofs << "\n";
    }
    ofs << "===================\n";
    ofs.close();
    cout << "\nCFG grammar has been written to " << filename << endl;
}

void writeParsingInfoToFile(const vector<Production>& grammar,
                            const map<string, set<string>> &first,
                            const map<string, set<string>> &follow,
                            const map<pair<string, string>, vector<int>> &parsingTable) {
    ofstream ofs("parsing_table.txt");
    if (!ofs) {
        cerr << "Error opening output file." << endl;
        return;
    }
    
    ofs << "=== FIRST Sets ===\n";
    for (const auto &p : first) {
        if (!isNonTerminal(p.first) && p.first != EPSILON)
            continue;
        ofs << p.first << " : { ";
        for (const auto &sym : p.second)
            ofs << sym << " ";
        ofs << "}\n";
    }
    
    ofs << "\n=== FOLLOW Sets ===\n";
    for (const auto &p : follow) {
        ofs << p.first << " : { ";
        for (const auto &sym : p.second)
            ofs << sym << " ";
        ofs << "}\n";
    }
    
    ofs << "\n=== Parsing Table ===\n";
    for (const auto &entry : parsingTable) {
        const auto &key = entry.first;
        const auto &prodIndices = entry.second;
        ofs << "M[" << key.first << ", " << key.second << "] = ";
        for (int idx : prodIndices) {
            const Production &prod = grammar[idx];
            ofs << prod.lhs << " -> ";
            for (const auto &sym : prod.rhs)
                ofs << sym << " ";
            ofs << "\n";
        }
    }
    
    ofs.close();
    cout << "\nFIRST sets, FOLLOW sets, and parsing table written to parsing_table.txt" << endl;
}

void generateSymbolTableFile(const vector<Token>& tokens, const string &filename) {
    // Separate hash tables for each token category.
    unordered_map<string, Token> identifierTable;   // For IDENTIFIER tokens.
    unordered_map<string, Token> numberTable;       // For NUMBER tokens.
    unordered_map<string, Token> stringTable;       // For STRING tokens.
    unordered_map<string, Token> keywordTable;      // For keywords.
    unordered_map<string, Token> symbolTable;       // For punctuation and operators.

    // Process each token and insert into the appropriate table.
    for (const auto &tok : tokens) {
        // Skip the END token.
        if (tok.type == TokenType::END) continue;
        
        switch (tok.type) {
            case TokenType::IDENTIFIER:
                identifierTable[tok.lexeme] = tok;
                break;
            case TokenType::NUMBER:
                numberTable[tok.lexeme] = tok;
                break;
            case TokenType::STRING:
                stringTable[tok.lexeme] = tok;
                break;
            // Group all keyword token types.
            case TokenType::CREATE:
            case TokenType::TABLE:
            case TokenType::PRIMARY:
            case TokenType::KEY:
            case TokenType::INSERT:
            case TokenType::INTO:
            case TokenType::VALUES:
            case TokenType::SELECT:
            case TokenType::FROM:
            case TokenType::WHERE:
            case TokenType::BETWEEN:
            case TokenType::AND:
            case TokenType::LIKE:
            case TokenType::IN:
                keywordTable[tok.lexeme] = tok;
                break;
            // For symbols such as punctuation and operators.
            case TokenType::LPAREN:
            case TokenType::RPAREN:
            case TokenType::COMMA:
            case TokenType::SEMICOLON:
            case TokenType::ASTERISK:
            case TokenType::EQUAL:
            case TokenType::GREATER:
            case TokenType::LESS:
                symbolTable[tok.lexeme] = tok;
                break;
            default:
                break;
        }
    }
    
    // Open file for writing the symbol table.
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        return;
    }
    
    ofs << "=== Symbol Table ===\n";
    
    ofs << "\n--- Identifiers ---\n";
    for (const auto &entry : identifierTable) {
        ofs << entry.first << "\t(" << tokenName(entry.second.type) << ")\n";
    }
    
    ofs << "\n--- Numbers ---\n";
    for (const auto &entry : numberTable) {
        ofs << entry.first << "\t(" << tokenName(entry.second.type) << ")\n";
    }
    
    ofs << "\n--- String Literals ---\n";
    for (const auto &entry : stringTable) {
        ofs << entry.first << "\t(" << tokenName(entry.second.type) << ")\n";
    }
    
    ofs << "\n--- Keywords ---\n";
    for (const auto &entry : keywordTable) {
        ofs << entry.first << "\t(" << tokenName(entry.second.type) << ")\n";
    }
    
    ofs << "\n--- Symbols ---\n";
    for (const auto &entry : symbolTable) {
        ofs << entry.first << "\t(" << tokenName(entry.second.type) << ")\n";
    }
    
    ofs << "\n=====================\n";
    ofs.close();
    
    cout << "\nSymbol table has been written to " << filename << endl;
}