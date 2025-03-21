#include <iostream>
#include <vector>
#include "grammar.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"
using namespace std;

int main() {
    // Print the CFG for reference.
    cout << "=== Context-Free Grammar (CFG) for Simplified SQL ===" << endl;
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
)CFG" << endl;
    
    cout << "Enter SQL query (end with an empty line):" << endl;
    string input, line;
    while(getline(cin, line)) {
        if(line.empty()) break;
        input += line + "\n";
    }
    
    // Lexical Analysis: Tokenize the input SQL query.
    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    
    cout << "\n=== Token Table ===" << endl;
    for (const auto &tok : tokens) {
        cout << tok.lexeme << "\t(" << tokenName(tok.type) << ")" << endl;
    }
    cout << "===================\n\n";
    
    // Build the grammar and compute FIRST, FOLLOW, and the LL(1) parsing table.
    vector<Production> grammar = buildGrammar();
    generateCFGGrammarFile(grammar, "CFG_grammar.txt");
    auto first = computeFirstSets(grammar);
    auto follow = computeFollowSets(grammar, first);
    auto parsingTable = constructParsingTable(grammar, first, follow);
    writeParsingInfoToFile(grammar, first, follow, parsingTable);
    
    // Perform syntax analysis using the LL(1) parsing table.
    cout << "\n=== LL(1) Parsing (Using Parsing Table) ===" << endl;
    ll1Parse(tokens, grammar, parsingTable);
    cout << "=============================================" << endl;
    
    return 0;
}
