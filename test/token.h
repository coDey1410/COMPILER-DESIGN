#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

// Token types for SQL.
enum class TokenType {
    CREATE,
    TABLE,
    PRIMARY,
    KEY,
    INSERT,
    INTO,
    VALUES,
    SELECT,
    FROM,
    WHERE,
    BETWEEN,
    AND,
    LIKE,
    IN,
    LPAREN,
    RPAREN,
    COMMA,
    SEMICOLON,
    ASTERISK,
    EQUAL,
    GREATER,
    LESS,
    IDENTIFIER,
    NUMBER,
    STRING,
    ERROR, 
    END
};

// Structure representing a token.
struct Token {
    TokenType type;
    string lexeme;
};

// Converts a TokenType value to a human-readable string.
inline string tokenName(TokenType t) {
    switch(t) {
        case TokenType::CREATE:      return "CREATE";
        case TokenType::TABLE:       return "TABLE";
        case TokenType::PRIMARY:     return "PRIMARY";
        case TokenType::KEY:         return "KEY";
        case TokenType::INSERT:      return "INSERT";
        case TokenType::INTO:        return "INTO";
        case TokenType::VALUES:      return "VALUES";
        case TokenType::SELECT:      return "SELECT";
        case TokenType::FROM:        return "FROM";
        case TokenType::WHERE:       return "WHERE";
        case TokenType::BETWEEN:     return "BETWEEN";
        case TokenType::AND:         return "AND";
        case TokenType::LIKE:        return "LIKE";
        case TokenType::IN:          return "IN";
        case TokenType::LPAREN:      return "(";
        case TokenType::RPAREN:      return ")";
        case TokenType::COMMA:       return ",";
        case TokenType::SEMICOLON:   return ";";
        case TokenType::ASTERISK:    return "*";
        case TokenType::EQUAL:       return "=";
        case TokenType::GREATER:     return ">";
        case TokenType::LESS:        return "<";
        case TokenType::IDENTIFIER:  return "IDENTIFIER";
        case TokenType::NUMBER:      return "NUMBER";
        case TokenType::STRING:      return "STRING";
        case TokenType::END:         return "END";
        default:                     return "UNKNOWN";
    }
}

#endif // TOKEN_H
