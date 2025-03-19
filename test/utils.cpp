#include "utils.h"
#include <fstream>
#include <iostream>
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
