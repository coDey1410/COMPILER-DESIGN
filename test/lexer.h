#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
using namespace std;

// Token types for SQL.
enum class TokenType {
    CREATE, TABLE, PRIMARY, KEY, INSERT, INTO, VALUES, SELECT, FROM, WHERE,
    BETWEEN, AND, LIKE, IN,
    LPAREN, RPAREN, COMMA, SEMICOLON, ASTERISK,
    EQUAL, GREATER, LESS,
    IDENTIFIER, NUMBER, STRING,
    END
};

// Token structure: type and lexeme.
struct Token {
    TokenType type;
    string lexeme;
};

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
