#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <set>
#include <fstream>

using namespace std;

// ===========================
// Part 1: Grammar Representation
// ===========================

struct Production {
    string lhs;
    vector<string> rhs;
};

const string EPSILON = "EPSILON";

// Helper: Return true if symbol is nonterminal (we assume nonterminals are enclosed in angle brackets)
bool isNonTerminal(const string &s) {
    return !s.empty() && s.front() == '<' && s.back() == '>';
}

// Build the grammar (an LL(1) version) as a vector of productions.
vector<Production> buildGrammar() {
    vector<Production> grammar;
    // 1. <sql> -> <statement> ";" <sql_tail>
    grammar.push_back({"<sql>", {"<statement>", ";", "<sql_tail>"}});
    // 2. <sql_tail> -> <statement> ";" <sql_tail>
    grammar.push_back({"<sql_tail>", {"<statement>", ";", "<sql_tail>"}});
    // 3. <sql_tail> -> EPSILON
    grammar.push_back({"<sql_tail>", {EPSILON}});
    
    // 4. <statement> -> <create_table_stmt>
    grammar.push_back({"<statement>", {"<create_table_stmt>"}});
    // 5. <statement> -> <insert_stmt>
    grammar.push_back({"<statement>", {"<insert_stmt>"}});
    // 6. <statement> -> <select_stmt>
    grammar.push_back({"<statement>", {"<select_stmt>"}});
    
    // 7. <create_table_stmt> -> "CREATE" "TABLE" <identifier> "(" <column_def_list> "," "PRIMARY" "KEY" "(" <identifier> ")" ")"
    grammar.push_back({"<create_table_stmt>", {"CREATE", "TABLE", "<identifier>", "(", "<column_def_list>", ",", "PRIMARY", "KEY", "(", "<identifier>", ")", ")"}});
    
    // 8. <column_def_list> -> <column_def> <column_def_list_tail>
    grammar.push_back({"<column_def_list>", {"<column_def>", "<column_def_list_tail>"}});
    // 9. <column_def_list_tail> -> "," <column_def> <column_def_list_tail>
    grammar.push_back({"<column_def_list_tail>", {",", "<column_def>", "<column_def_list_tail>"}});
    // 10. <column_def_list_tail> -> EPSILON
    grammar.push_back({"<column_def_list_tail>", {EPSILON}});
    
    // 11. <column_def> -> <identifier> <datatype>
    grammar.push_back({"<column_def>", {"<identifier>", "<datatype>"}});
    
    // 12. <insert_stmt> -> "INSERT" "INTO" <identifier> <opt_identifier_list> "VALUES" "(" <value_list> ")"
    grammar.push_back({"<insert_stmt>", {"INSERT", "INTO", "<identifier>", "<opt_identifier_list>", "VALUES", "(", "<value_list>", ")"}});
    // 13. <opt_identifier_list> -> "(" <identifier_list> ")"
    grammar.push_back({"<opt_identifier_list>", {"(", "<identifier_list>", ")"}});
    // 14. <opt_identifier_list> -> EPSILON
    grammar.push_back({"<opt_identifier_list>", {EPSILON}});
    
    // 15. <identifier_list> -> <identifier> <identifier_list_tail>
    grammar.push_back({"<identifier_list>", {"<identifier>", "<identifier_list_tail>"}});
    // 16. <identifier_list_tail> -> "," <identifier> <identifier_list_tail>
    grammar.push_back({"<identifier_list_tail>", {",", "<identifier>", "<identifier_list_tail>"}});
    // 17. <identifier_list_tail> -> EPSILON
    grammar.push_back({"<identifier_list_tail>", {EPSILON}});
    
    // 18. <value_list> -> <value> <value_list_tail>
    grammar.push_back({"<value_list>", {"<value>", "<value_list_tail>"}});
    // 19. <value_list_tail> -> "," <value> <value_list_tail>
    grammar.push_back({"<value_list_tail>", {",", "<value>", "<value_list_tail>"}});
    // 20. <value_list_tail> -> EPSILON
    grammar.push_back({"<value_list_tail>", {EPSILON}});
    
    // 21. <select_stmt> -> "SELECT" <select_list> "FROM" <identifier> <opt_where_clause> ";"
    grammar.push_back({"<select_stmt>", {"SELECT", "<select_list>", "FROM", "<identifier>", "<opt_where_clause>", ";"}});

    // 22. <select_list> -> "*"
    grammar.push_back({"<select_list>", {"*"}});
    // 23. <select_list> -> <identifier_list>
    grammar.push_back({"<select_list>", {"<identifier_list>"}});
    
    // 24. <opt_where_clause> -> <where_clause>
    grammar.push_back({"<opt_where_clause>", {"<where_clause>"}});
    // 25. <opt_where_clause> -> EPSILON
    grammar.push_back({"<opt_where_clause>", {EPSILON}});
    
    // 26. <where_clause> -> "WHERE" <condition>
    grammar.push_back({"<where_clause>", {"WHERE", "<condition>"}});
    
    // New production: <condition> -> <identifier> <condition_tail>
grammar.push_back({"<condition>", {"<identifier>", "<condition_tail>"}});

// Define <condition_tail> alternatives:
// <condition_tail> -> <comp_operator> <value>
grammar.push_back({"<condition_tail>", {"<comp_operator>", "<value>"}});

// <condition_tail> -> BETWEEN <value> AND <value>
grammar.push_back({"<condition_tail>", {"BETWEEN", "<value>", "AND", "<value>"}});

// <condition_tail> -> LIKE <value>
grammar.push_back({"<condition_tail>", {"LIKE", "<value>"}});

// <condition_tail> -> IN "(" <value_list> ")"
grammar.push_back({"<condition_tail>", {"IN", "(", "<value_list>", ")"}});

    // 31. <comp_operator> -> "="
    grammar.push_back({"<comp_operator>", {"="}});
    // 32. <comp_operator> -> ">"
    grammar.push_back({"<comp_operator>", {">"}});
    // 33. <comp_operator> -> "<"
    grammar.push_back({"<comp_operator>", {"<"}});
    
    // 34. <identifier> -> id      (a placeholder for an identifier token)
    grammar.push_back({"<identifier>", {"id"}});
    // 35. <datatype> -> <identifier>
    grammar.push_back({"<datatype>", {"<identifier>"}});
    
    // 36. <value> -> <number>
    grammar.push_back({"<value>", {"<number>"}});
    // 37. <value> -> <string>
    grammar.push_back({"<value>", {"<string>"}});
    // 38. <value> -> <identifier>
    grammar.push_back({"<value>", {"<identifier>"}});
    
    // 39. <number> -> num      (a placeholder for a numeric literal)
    grammar.push_back({"<number>", {"num"}});
    // 40. <string> -> str      (a placeholder for a string literal)
    grammar.push_back({"<string>", {"str"}});
    
    return grammar;
}

// ===========================
// Part 2: FIRST and FOLLOW Set Computation and Parsing Table Construction
// ===========================

// Compute FIRST sets for all grammar symbols.
map<string, set<string>> computeFirstSets(const vector<Production>& grammar) {
    map<string, set<string>> first;
    
    // Initialize FIRST for terminals (nonterminals will be computed later)
    auto addTerminalFirst = [&](const string &sym) {
        if (!isNonTerminal(sym) && sym != EPSILON)
            first[sym].insert(sym);
    };
    for (const auto &prod : grammar) {
        // Ensure lhs is in the table
        if (first.find(prod.lhs) == first.end())
            first[prod.lhs] = set<string>();
        for (const auto &sym : prod.rhs) {
            addTerminalFirst(sym);
        }
    }
    
    // For EPSILON:
    first[EPSILON].insert(EPSILON);
    
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto &prod : grammar) {
            const string &A = prod.lhs;
            set<string> firstAlpha;
            bool allEpsilon = true;
            for (const auto &sym : prod.rhs) {
                for (const auto &item : first[sym]) {
                    if (item != EPSILON)
                        firstAlpha.insert(item);
                }
                if (first[sym].find(EPSILON) == first[sym].end()) {
                    allEpsilon = false;
                    break;
                }
            }
            if (allEpsilon)
                firstAlpha.insert(EPSILON);
            
            size_t before = first[A].size();
            first[A].insert(firstAlpha.begin(), firstAlpha.end());
            if (first[A].size() != before)
                changed = true;
        }
    }
    return first;
}

// Compute FOLLOW sets for all nonterminals.
map<string, set<string>> computeFollowSets(const vector<Production>& grammar, const map<string, set<string>> &first) {
    map<string, set<string>> follow;
    
    for (const auto &prod : grammar) {
        if (isNonTerminal(prod.lhs))
            follow[prod.lhs]; // default empty set
    }
    
    // Add "$" (end-marker) to FOLLOW of start symbol <sql>
    follow["<sql>"].insert("$");
    
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto &prod : grammar) {
            const string &A = prod.lhs;
            for (size_t i = 0; i < prod.rhs.size(); i++) {
                string B = prod.rhs[i];
                if (isNonTerminal(B)) {
                    size_t before = follow[B].size();
                    bool betaAllEpsilon = true;
                    for (size_t j = i + 1; j < prod.rhs.size(); j++) {
                        const string &betaSym = prod.rhs[j];
                        for (const auto &item : first.at(betaSym)) {
                            if (item != EPSILON)
                                follow[B].insert(item);
                        }
                        if (first.at(betaSym).find(EPSILON) == first.at(betaSym).end()) {
                            betaAllEpsilon = false;
                            break;
                        }
                    }
                    if (betaAllEpsilon) {
                        follow[B].insert(follow[A].begin(), follow[A].end());
                    }
                    if (follow[B].size() != before)
                        changed = true;
                }
            }
        }
    }
    return follow;
}

// Construct the LL(1) parsing table.
// The table maps (nonterminal, terminal) pairs to a vector of production indices.
map<pair<string, string>, vector<int>> constructParsingTable(const vector<Production>& grammar,
                                                              const map<string, set<string>> &first,
                                                              const map<string, set<string>> &follow) {
    map<pair<string, string>, vector<int>> table;
    for (int i = 0; i < grammar.size(); i++) {
        const Production &prod = grammar[i];
        set<string> firstAlpha;
        bool allEpsilon = true;
        for (const auto &sym : prod.rhs) {
            for (const auto &item : first.at(sym)) {
                if (item != EPSILON)
                    firstAlpha.insert(item);
            }
            if (first.at(sym).find(EPSILON) == first.at(sym).end()) {
                allEpsilon = false;
                break;
            }
        }
        if (allEpsilon)
            firstAlpha.insert(EPSILON);
        
        for (const auto &term : firstAlpha) {
            if (term != EPSILON) {
                table[{prod.lhs, term}].push_back(i);
            }
        }
        if (firstAlpha.find(EPSILON) != firstAlpha.end()) {
            for (const auto &b : follow.at(prod.lhs)) {
                table[{prod.lhs, b}].push_back(i);
            }
        }
    }
    return table;
}

// Write FIRST sets, FOLLOW sets, and the parsing table to a text file.
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

// ===========================
// Part 3: Lexer, LL(1) Parser Using Parsing Table, and Parse Tree Generation
// ===========================

// --- Token Definitions for SQL ---
enum class TokenType {
    // Keywords
    CREATE, TABLE, PRIMARY, KEY, INSERT, INTO, VALUES, SELECT, FROM, WHERE,
    BETWEEN, AND, LIKE, IN,
    // Symbols
    LPAREN, RPAREN, COMMA, SEMICOLON, ASTERISK,
    // Operators
    EQUAL, GREATER, LESS,
    // Identifier and literal types
    IDENTIFIER, NUMBER, STRING,
    // End marker
    END
};

struct Token {
    TokenType type;
    string lexeme;
};

// --- Lexical Analyzer ---
class Lexer {
public:
    Lexer(const string &input) : input(input), pos(0) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (true) {
            skipWhitespace();
            if (pos >= input.size()) {
                tokens.push_back({TokenType::END, ""});
                break;
            }
            char current = input[pos];
            if (isalpha(current) || current == '_') {
                string word = readWord();
                Token token = keywordOrIdentifier(word);
                tokens.push_back(token);
            } else if (isdigit(current)) {
                string num = readNumber();
                tokens.push_back({TokenType::NUMBER, num});
            } else if (current == '\'' || current == '\"') {
                string str = readString();
                tokens.push_back({TokenType::STRING, str});
            } else {
                switch (current) {
                    case '(': tokens.push_back({TokenType::LPAREN, "("}); pos++; break;
                    case ')': tokens.push_back({TokenType::RPAREN, ")"}); pos++; break;
                    case ',': tokens.push_back({TokenType::COMMA, ","}); pos++; break;
                    case ';': tokens.push_back({TokenType::SEMICOLON, ";"}); pos++; break;
                    case '*': tokens.push_back({TokenType::ASTERISK, "*"}); pos++; break;
                    case '=': tokens.push_back({TokenType::EQUAL, "="}); pos++; break;
                    case '>': tokens.push_back({TokenType::GREATER, ">"}); pos++; break;
                    case '<': tokens.push_back({TokenType::LESS, "<"}); pos++; break;
                    default:
                        cerr << "Unknown character: " << current << endl;
                        pos++;
                        break;
                }
            }
        }
        return tokens;
    }
private:
    string input;
    size_t pos;
    void skipWhitespace() {
        while (pos < input.size() && isspace(input[pos]))
            pos++;
    }
    string readWord() {
        size_t start = pos;
        while (pos < input.size() && (isalnum(input[pos]) || input[pos]=='_'))
            pos++;
        return input.substr(start, pos - start);
    }
    string readNumber() {
        size_t start = pos;
        while (pos < input.size() && isdigit(input[pos]))
            pos++;
        if (pos < input.size() && input[pos]=='.') {
            pos++;
            while (pos < input.size() && isdigit(input[pos]))
                pos++;
        }
        return input.substr(start, pos - start);
    }
    string readString() {
        char quote = input[pos];
        pos++; // skip opening
        size_t start = pos;
        while (pos < input.size() && input[pos]!=quote)
            pos++;
        string str = input.substr(start, pos - start);
        pos++; // skip closing
        return str;
    }
    Token keywordOrIdentifier(const string &word) {
        string upperWord = word;
        transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
        if (upperWord=="CREATE") return {TokenType::CREATE, word};
        if (upperWord=="TABLE")  return {TokenType::TABLE, word};
        if (upperWord=="PRIMARY") return {TokenType::PRIMARY, word};
        if (upperWord=="KEY") return {TokenType::KEY, word};
        if (upperWord=="INSERT") return {TokenType::INSERT, word};
        if (upperWord=="INTO") return {TokenType::INTO, word};
        if (upperWord=="VALUES") return {TokenType::VALUES, word};
        if (upperWord=="SELECT") return {TokenType::SELECT, word};
        if (upperWord=="FROM") return {TokenType::FROM, word};
        if (upperWord=="WHERE") return {TokenType::WHERE, word};
        if (upperWord=="BETWEEN") return {TokenType::BETWEEN, word};
        if (upperWord=="AND") return {TokenType::AND, word};
        if (upperWord=="LIKE") return {TokenType::LIKE, word};
        if (upperWord=="IN") return {TokenType::IN, word};
        return {TokenType::IDENTIFIER, word};
    }
};

// --- LL(1) Parser Using Parsing Table ---
// This function implements the standard LL(1) parsing algorithm using a stack and the parsing table.
void ll1Parse(const vector<Token>& tokens, const vector<Production>& grammar,
              const map<pair<string, string>, vector<int>> &parsingTable) {
    // Use a stack of strings representing grammar symbols.
    // Initialize the stack with "$" (end-marker) and the start symbol.
    vector<string> stack;
    stack.push_back("$");
    stack.push_back("<sql>");
    
    int i = 0;  // index into tokens

    while (!stack.empty()) {
        string top = stack.back();
        // Determine the current input symbol.
        string inputSymbol;
        if (tokens[i].type == TokenType::END)
            inputSymbol = "$";
        else {
            // For tokens corresponding to placeholders, map to our grammar terminals.
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
            // Top is a nonterminal; look up the parsing table entry.
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
            // Pop the nonterminal.
            stack.pop_back();
            // If production is EPSILON, do nothing; otherwise push RHS in reverse.
            if (!(prod.rhs.size() == 1 && prod.rhs[0] == EPSILON)) {
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

// ===========================
// Utility Function to Write CFG Grammar to File
// ===========================
void generateCFGGrammarFile(const vector<Production>& grammar, const string &filename) {
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        return;
    }
    ofs << "=== CFG Grammar ===\n";
    for (const auto &prod : grammar) {
        ofs << prod.lhs << " ::= ";
        for (const auto &sym : prod.rhs) {
            ofs << sym << " ";
        }
        ofs << "\n";
    }
    ofs << "===================\n";
    ofs.close();
    cout << "\nCFG grammar has been written to " << filename << endl;
}

// ===========================
// Main Function
// ===========================
int main() {
    // Print the CFG for reference.
    cout << "=== Context-Free Grammar (CFG) for Simplified SQL ===\n";
    cout << R"CFG(
<sql>         ::= <statement> ";" { <statement> ";" }
<statement>   ::= <create_table_stmt> | <insert_stmt> | <select_stmt>
<create_table_stmt>  ::= "CREATE" "TABLE" <identifier> "(" <column_def_list> "," "PRIMARY" "KEY" "(" <identifier> ")" ")"
<column_def_list>    ::= <column_def> { "," <column_def> }
<column_def>         ::= <identifier> <datatype>
<insert_stmt>  ::= "INSERT" "INTO" <identifier> [ "(" <identifier_list> ")" ] "VALUES" "(" <value_list> ")"
<identifier_list>    ::= <identifier> { "," <identifier> }
<value_list>         ::= <value> { "," <value> }
<select_stmt>  ::= "SELECT" <select_list> "FROM" <identifier> [ <where_clause> ] ";"
<select_list>  ::= "*" | <identifier_list>
<where_clause> ::= "WHERE" <condition>
<condition>    ::= <identifier> <comp_operator> <value>
                |  <identifier> "BETWEEN" <value> "AND" <value>
                |  <identifier> "LIKE" <value>
                |  <identifier> "IN" "(" <value_list> ")"
<comp_operator>  ::= "=" | ">" | "<"
<identifier>   ::= id
<datatype>     ::= <identifier>
<value>        ::= <number> | <string> | <identifier>
<number>       ::= num
<string>       ::= str
)CFG" << "\n=====================================================\n\n";
    
    cout << "Enter SQL query (end with an empty line):" << endl;
    string input, line;
    while(getline(cin, line)) {
        if(line.empty()) break;
        input += line + "\n";
    }
    
    // Lexical Analysis
    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    
    cout << "\n=== Token Table ===" << endl;
    for (const auto &tok : tokens) {
        // For display purposes, print the token lexeme.
        cout << tok.lexeme << "\t(" << static_cast<int>(tok.type) << ")" << endl;
    }
    cout << "===================\n\n";
    
    // Build Grammar and compute FIRST, FOLLOW, and Parsing Table
    vector<Production> grammar = buildGrammar();
    generateCFGGrammarFile(grammar, "CFG_grammar.txt");
    auto first = computeFirstSets(grammar);
    auto follow = computeFollowSets(grammar, first);
    auto parsingTable = constructParsingTable(grammar, first, follow);
    writeParsingInfoToFile(grammar, first, follow, parsingTable);
    
    // Syntax Analysis using LL(1) Parsing Table
    cout << "\n=== LL(1) Parsing (Using Parsing Table) ===" << endl;
    ll1Parse(tokens, grammar, parsingTable);
    cout << "=============================================" << endl;
    
    return 0;
}
