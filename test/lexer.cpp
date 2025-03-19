#include "lexer.h"
#include <cctype>
#include <iostream>
#include <algorithm>

Lexer::Lexer(const string &input) : input(input), pos(0) {}

void Lexer::skipWhitespace() {
    while (pos < input.size() && isspace(input[pos]))
        pos++;
}

string Lexer::readWord() {
    size_t start = pos;
    while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_'))
        pos++;
    return input.substr(start, pos - start);
}

string Lexer::readNumber() {
    size_t start = pos;
    while (pos < input.size() && isdigit(input[pos]))
        pos++;
    if (pos < input.size() && input[pos] == '.') {
        pos++;
        while (pos < input.size() && isdigit(input[pos]))
            pos++;
    }
    return input.substr(start, pos - start);
}

string Lexer::readString() {
    char quote = input[pos];
    pos++; // Skip opening quote.
    size_t start = pos;
    while (pos < input.size() && input[pos] != quote)
        pos++;
    string str = input.substr(start, pos - start);
    pos++; // Skip closing quote.
    return str;
}

Token Lexer::keywordOrIdentifier(const string &word) {
    string upperWord = word;
    transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
    if (upperWord == "CREATE") return {TokenType::CREATE, word};
    if (upperWord == "TABLE")  return {TokenType::TABLE, word};
    if (upperWord == "PRIMARY") return {TokenType::PRIMARY, word};
    if (upperWord == "KEY") return {TokenType::KEY, word};
    if (upperWord == "INSERT") return {TokenType::INSERT, word};
    if (upperWord == "INTO") return {TokenType::INTO, word};
    if (upperWord == "VALUES") return {TokenType::VALUES, word};
    if (upperWord == "SELECT") return {TokenType::SELECT, word};
    if (upperWord == "FROM") return {TokenType::FROM, word};
    if (upperWord == "WHERE") return {TokenType::WHERE, word};
    if (upperWord == "BETWEEN") return {TokenType::BETWEEN, word};
    if (upperWord == "AND") return {TokenType::AND, word};
    if (upperWord == "LIKE") return {TokenType::LIKE, word};
    if (upperWord == "IN") return {TokenType::IN, word};
    return {TokenType::IDENTIFIER, word};
}

vector<Token> Lexer::tokenize() {
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
