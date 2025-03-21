#ifndef LEXER_H
#define LEXER_H
#include "token.h"  
#include <string>
#include <vector>
using namespace std;




// Token structure: type and lexeme.


class Lexer {
public:
    Lexer(const string &input);
    vector<Token> tokenize();
private:
    string input;
    size_t pos;
    void skipWhitespace();
    string readWord();
    string readNumber();
    string readString();
    Token keywordOrIdentifier(const string &word);
};

#endif // LEXER_H
