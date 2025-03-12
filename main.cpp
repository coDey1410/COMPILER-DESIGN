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
// Note: Some EBNF constructs have been expanded into additional nonterminals.
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
    
    // 27. <condition> -> <identifier> <comp_operator> <value>
    grammar.push_back({"<condition>", {"<identifier>", "<comp_operator>", "<value>"}});
    // 28. <condition> -> <identifier> "BETWEEN" <value> "AND" <value>
    grammar.push_back({"<condition>", {"<identifier>", "BETWEEN", "<value>", "AND", "<value>"}});
    // 29. <condition> -> <identifier> "LIKE" <value>
    grammar.push_back({"<condition>", {"<identifier>", "LIKE", "<value>"}});
    // 30. <condition> -> <identifier> "IN" "(" <value_list> ")"
    grammar.push_back({"<condition>", {"<identifier>", "IN", "(", "<value_list>", ")"}});
    
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
            // Compute FIRST(alpha) for production A -> alpha
            set<string> firstAlpha;
            bool allEpsilon = true;
            for (const auto &sym : prod.rhs) {
                // If sym is a terminal, then FIRST(sym) = {sym}.
                // Otherwise, it's a nonterminal.
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
            
            // Add FIRST(alpha) to FIRST(A)
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
    
    // Initialize FOLLOW for each nonterminal
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
                    // Compute FIRST(beta)
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
                        // Add FOLLOW(A) to FOLLOW(B)
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
// The table maps (nonterminal, terminal) pairs to a vector of production indices (for conflicts).
map<pair<string, string>, vector<int>> constructParsingTable(const vector<Production>& grammar,
                                                              const map<string, set<string>> &first,
                                                              const map<string, set<string>> &follow) {
    map<pair<string, string>, vector<int>> table;
    for (int i = 0; i < grammar.size(); i++) {
        const Production &prod = grammar[i];
        // Compute FIRST(alpha) for production prod: A -> alpha.
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
        
        // For each terminal 'a' in FIRST(alpha), add production i to table[A, a]
        for (const auto &term : firstAlpha) {
            if (term != EPSILON) {
                table[{prod.lhs, term}].push_back(i);
            }
        }
        // If EPSILON is in FIRST(alpha), add production i to table[A, b] for every b in FOLLOW(A)
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
            continue; // Print FIRST only for nonterminals.
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
            // Print the production in the form: A -> alpha
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
// Part 3: Lexer, Parser, and Parse Tree (as before)
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

// --- Parse Tree Node Definition ---
struct Node {
    string label;
    vector<Node*> children;
};

Node* newNode(const string &label) {
    Node* node = new Node();
    node->label = label;
    return node;
}

// Helper to print a token as a terminal node.
string tokenName(TokenType t) {
    switch(t) {
        case TokenType::CREATE: return "CREATE";
        case TokenType::TABLE: return "TABLE";
        case TokenType::PRIMARY: return "PRIMARY";
        case TokenType::KEY: return "KEY";
        case TokenType::INSERT: return "INSERT";
        case TokenType::INTO: return "INTO";
        case TokenType::VALUES: return "VALUES";
        case TokenType::SELECT: return "SELECT";
        case TokenType::FROM: return "FROM";
        case TokenType::WHERE: return "WHERE";
        case TokenType::BETWEEN: return "BETWEEN";
        case TokenType::AND: return "AND";
        case TokenType::LIKE: return "LIKE";
        case TokenType::IN: return "IN";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::COMMA: return ",";
        case TokenType::SEMICOLON: return ";";
        case TokenType::ASTERISK: return "*";
        case TokenType::EQUAL: return "=";
        case TokenType::GREATER: return ">";
        case TokenType::LESS: return "<";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::END: return "END";
        default: return "UNKNOWN";
    }
}

// --- Recursive Descent Parser with Parse Tree Generation ---
class Parser {
public:
    Parser(const vector<Token>& tokens) : tokens(tokens), pos(0) {}
    Node* parse() {
        return parseSql();
    }
private:
    vector<Token> tokens;
    size_t pos;
    Token peek() { return pos < tokens.size() ? tokens[pos] : Token{TokenType::END, ""}; }
    Token advance() { return pos < tokens.size() ? tokens[pos++] : Token{TokenType::END, ""}; }
    bool match(TokenType expected) {
        if (peek().type==expected) { advance(); return true; }
        return false;
    }
    void error(const string &msg) {
        cerr << "Syntax Error: " << msg << " at token '" << peek().lexeme << "'" << endl;
        exit(1);
    }
    Token expect(TokenType expected) {
        if (peek().type==expected)
            return advance();
        error("Expected " + tokenName(expected));
        return Token{TokenType::END, ""}; // unreachable
    }
    Node* terminal(Token token) {
        return newNode("[" + tokenName(token.type) + ": " + token.lexeme + "]");
    }
    // Grammar parsing functions (matching the earlier recursive descent design)
    Node* parseSql() {
        Node* node = newNode("<sql>");
        while (peek().type != TokenType::END) {
            Node* stmt = parseStatement();
            node->children.push_back(stmt);
        }
        return node;
    }
    
    
    Node* parseSqlTail() {
        Node* node = newNode("<sql_tail>");
        if (peek().type == TokenType::END) {
            node->children.push_back(newNode(EPSILON));
            return node;
        }
        // Lookahead: if next tokens can start a statement, parse tail.
        if (peek().type==TokenType::CREATE || peek().type==TokenType::INSERT || peek().type==TokenType::SELECT) {
            Node* stmt = parseStatement();
            node->children.push_back(stmt);
            Token t = expect(TokenType::SEMICOLON); // Changed from expect(';')
            node->children.push_back(terminal(t));
            Node* tail = parseSqlTail();
            node->children.push_back(tail);
        } else {
            node->children.push_back(newNode(EPSILON));
        }
        return node;
    }
        Node* parseStatement() {
        Node* node = newNode("<statement>");
        Token current = peek();
        if (current.type==TokenType::CREATE)
            node->children.push_back(parseCreateTable());
        else if (current.type==TokenType::INSERT)
            node->children.push_back(parseInsert());
        else if (current.type==TokenType::SELECT)
            node->children.push_back(parseSelect());
        else
            error("Expected CREATE, INSERT, or SELECT");
        return node;
    }
    Node* parseCreateTable() {
        Node* node = newNode("<create_table_stmt>");
        Token t = expect(TokenType::CREATE); node->children.push_back(terminal(t));
        t = expect(TokenType::TABLE); node->children.push_back(terminal(t));
        t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        t = expect(TokenType::LPAREN); node->children.push_back(terminal(t));
        node->children.push_back(parseColumnDefList());
        t = expect(TokenType::COMMA); node->children.push_back(terminal(t));
        t = expect(TokenType::PRIMARY); node->children.push_back(terminal(t));
        t = expect(TokenType::KEY); node->children.push_back(terminal(t));
        t = expect(TokenType::LPAREN); node->children.push_back(terminal(t));
        t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        t = expect(TokenType::RPAREN); node->children.push_back(terminal(t));
        t = expect(TokenType::RPAREN); node->children.push_back(terminal(t));
        t = expect(TokenType::SEMICOLON); node->children.push_back(terminal(t));
        return node;
    }
    Node* parseColumnDefList() {
        Node* node = newNode("<column_def_list>");
        node->children.push_back(parseColumnDef());
        while (peek().type==TokenType::COMMA) {
            // Stop if next is PRIMARY (for PRIMARY KEY clause)
            if (tokens.size() > pos+1 && tokens[pos+1].type==TokenType::PRIMARY)
                break;
            Token comma = expect(TokenType::COMMA);
            node->children.push_back(terminal(comma));
            node->children.push_back(parseColumnDef());
        }
        return node;
    }
    Node* parseColumnDef() {
        Node* node = newNode("<column_def>");
        Token t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        return node;
    }
    Node* parseInsert() {
        Node* node = newNode("<insert_stmt>");
        Token t = expect(TokenType::INSERT); node->children.push_back(terminal(t));
        t = expect(TokenType::INTO); node->children.push_back(terminal(t));
        t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        // Optional identifier list
        node->children.push_back(parseOptIdentifierList());
        t = expect(TokenType::VALUES); node->children.push_back(terminal(t));
        t = expect(TokenType::LPAREN); node->children.push_back(terminal(t));
        node->children.push_back(parseValueList());
        t = expect(TokenType::RPAREN); node->children.push_back(terminal(t));
        t = expect(TokenType::SEMICOLON); node->children.push_back(terminal(t));
        return node;
    }
    Node* parseOptIdentifierList() {
        Node* node = newNode("<opt_identifier_list>");
        if (peek().type==TokenType::LPAREN) {
            Token t = expect(TokenType::LPAREN); node->children.push_back(terminal(t));
            node->children.push_back(parseIdentifierList());
            t = expect(TokenType::RPAREN); node->children.push_back(terminal(t));
        } else {
            node->children.push_back(newNode(EPSILON));
        }
        return node;
    }
    Node* parseIdentifierList() {
        Node* node = newNode("<identifier_list>");
        Token t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        while (peek().type==TokenType::COMMA) {
            t = expect(TokenType::COMMA); node->children.push_back(terminal(t));
            t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        }
        return node;
    }
    Node* parseValueList() {
        Node* node = newNode("<value_list>");
        node->children.push_back(parseValue());
        while (peek().type==TokenType::COMMA) {
            Token t = expect(TokenType::COMMA); node->children.push_back(terminal(t));
            node->children.push_back(parseValue());
        }
        return node;
    }
    Node* parseValue() {
        Node* node = newNode("<value>");
        Token t = peek();
        if (t.type==TokenType::NUMBER || t.type==TokenType::STRING || t.type==TokenType::IDENTIFIER)
            node->children.push_back(terminal(advance()));
        else
            error("Expected value");
        return node;
    }
    Node* parseSelect() {
        Node* node = newNode("<select_stmt>");
        Token t = expect(TokenType::SELECT); node->children.push_back(terminal(t));
        node->children.push_back(parseSelectList());
        t = expect(TokenType::FROM); node->children.push_back(terminal(t));
        t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        node->children.push_back(parseOptWhereClause());
        t = expect(TokenType::SEMICOLON); node->children.push_back(terminal(t));
        return node;
    }
    Node* parseSelectList() {
        Node* node = newNode("<select_list>");
        if (peek().type==TokenType::ASTERISK) {
            Token t = expect(TokenType::ASTERISK); node->children.push_back(terminal(t));
        } else {
            node->children.push_back(parseIdentifierList());
        }
        return node;
    }
    Node* parseOptWhereClause() {
        Node* node = newNode("<opt_where_clause>");
        if (peek().type==TokenType::WHERE) {
            Token t = expect(TokenType::WHERE); node->children.push_back(terminal(t));
            node->children.push_back(parseCondition());
        } else {
            node->children.push_back(newNode(EPSILON));
        }
        return node;
    }
    Node* parseCondition() {
        Node* node = newNode("<condition>");
        Token t = expect(TokenType::IDENTIFIER); node->children.push_back(terminal(t));
        t = peek();
        if (t.type==TokenType::EQUAL || t.type==TokenType::GREATER || t.type==TokenType::LESS) {
            t = advance(); node->children.push_back(terminal(t));
            node->children.push_back(parseValue());
        } else if (t.type==TokenType::BETWEEN) {
            t = advance(); node->children.push_back(terminal(t));
            node->children.push_back(parseValue());
            t = expect(TokenType::AND); node->children.push_back(terminal(t));
            node->children.push_back(parseValue());
        } else if (t.type==TokenType::LIKE) {
            t = advance(); node->children.push_back(terminal(t));
            node->children.push_back(parseValue());
        } else if (t.type==TokenType::IN) {
            t = advance(); node->children.push_back(terminal(t));
            t = expect(TokenType::LPAREN); node->children.push_back(terminal(t));
            node->children.push_back(parseValueList());
            t = expect(TokenType::RPAREN); node->children.push_back(terminal(t));
        } else {
            error("Expected operator in condition");
        }
        return node;
    }
};

// Pre-order traversal to print the parse tree.
void printParseTree(Node* root, int indent = 0) {
    for (int i = 0; i < indent; i++) cout << "  ";
    cout << root->label << "\n";
    for (auto child : root->children)
        printParseTree(child, indent + 1);
}


// This function generates a text file containing the CFG derivation (parse tree) for the query.
// Function to generate a text file containing the CFG grammar.
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
        cout << tokenName(tok.type) << "\t" << tok.lexeme << endl;
    }
    cout << "===================\n\n";
    
    // Parsing and Parse Tree Generation
    Parser parser(tokens);
    Node* parseTree = parser.parse();
    //generateCFGDerivationFile(parseTree, "cfg_derivation.txt");

    cout << "\n=== Parse Tree ===" << endl;
    printParseTree(parseTree);
    cout << "==================\n\n";
    
    // Build Grammar and compute FIRST, FOLLOW, and Parsing Table
    vector<Production> grammar = buildGrammar();
    generateCFGGrammarFile(grammar, "CFG_grammar.txt");
    auto first = computeFirstSets(grammar);
    auto follow = computeFollowSets(grammar, first);
    auto parsingTable = constructParsingTable(grammar, first, follow);
    
    // Write the results to a text file.
    writeParsingInfoToFile(grammar, first, follow, parsingTable);
    
    return 0;
}
