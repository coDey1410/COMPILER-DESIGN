#include "lexer.h"
#include <cctype>
#include <iostream>
#include <algorithm>

Lexer::Lexer(const std::string &input) : input(input), pos(0) {}

void Lexer::skipWhitespace() {
    while (pos < input.size() && std::isspace(input[pos]))
        pos++;
}

std::string Lexer::readWord() {
    size_t start = pos;
    while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_'))
        pos++;
    return input.substr(start, pos - start);
}

std::string Lexer::readNumber() {
    size_t start = pos;
    bool dotSeen = false;
    while (pos < input.size() && (std::isdigit(input[pos]) || input[pos] == '.')) {
        if (input[pos] == '.') {
            if (dotSeen)
                break;  // Break out if multiple dots occur.
            dotSeen = true;
        }
        pos++;
    }
    return input.substr(start, pos - start);
}

std::string Lexer::readString() {
    char quote = input[pos];
    pos++; // Skip the opening quote.
    size_t start = pos;
    while (pos < input.size() && input[pos] != quote)
        pos++;
    std::string str = input.substr(start, pos - start);
    if (pos < input.size() && input[pos] == quote)
        pos++; // Skip the closing quote.
    else {
        // Report unterminated string to standard error.
        std::cerr << "Error: Unterminated string literal starting at position " << start - 1 << std::endl;
    }
    return str;
}

Token Lexer::keywordOrIdentifier(const std::string &word) {
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
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

// The DFA-based tokenize() function with error handling.
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    // Define DFA states.
    enum class State { START, IDENTIFIER, NUMBER, STRING };
    State state = State::START;
    std::string currentLexeme;
    char currentQuote = '\0';  // For string state.
    bool dotSeen = false;      // For tracking decimal points in numbers.

    while (pos < input.size()) {
        char c = input[pos];
        switch (state) {
            case State::START:
                if (std::isspace(c)) {  
                    // Skip whitespace.
                    pos++;
                } else if (std::isalpha(c) || c == '_') {
                    // Start building an identifier.
                    state = State::IDENTIFIER;
                    currentLexeme.clear();
                } else if (std::isdigit(c)) {
                    // Start building a number.
                    state = State::NUMBER;
                    currentLexeme.clear();
                    dotSeen = false;
                } else if (c == '\'' || c == '"') {
                    // Start a string literal.
                    state = State::STRING;
                    currentLexeme.clear();
                    currentQuote = c;
                    pos++; // Skip the opening quote.
                } else {
                    // Process single-character tokens (symbols, operators).
                    switch (c) {
                        case '(': tokens.push_back({TokenType::LPAREN, "("}); break;
                        case ')': tokens.push_back({TokenType::RPAREN, ")"}); break;
                        case ',': tokens.push_back({TokenType::COMMA, ","}); break;
                        case ';': tokens.push_back({TokenType::SEMICOLON, ";"}); break;
                        case '*': tokens.push_back({TokenType::ASTERISK, "*"}); break;
                        case '=': tokens.push_back({TokenType::EQUAL, "="}); break;
                        case '>': tokens.push_back({TokenType::GREATER, ">"}); break;
                        case '<': tokens.push_back({TokenType::LESS, "<"}); break;
                        default:
                            // Report unknown character as an error.
                            tokens.push_back({TokenType::ERROR, std::string("Unknown character: ") + c});
                            break;
                    }
                    pos++;  // Consume the character.
                }
                break;

            case State::IDENTIFIER:
                if (std::isalnum(c) || c == '_') {
                    // Continue reading the identifier.
                    currentLexeme.push_back(c);
                    pos++;
                } else {
                    // End of identifier: determine if it's a keyword or a plain identifier.
                    tokens.push_back(keywordOrIdentifier(currentLexeme));
                    state = State::START;
                }
                break;

            case State::NUMBER:
                if (std::isdigit(c)) {
                    currentLexeme.push_back(c);
                    pos++;
                } else if (c == '.') {
                    if (dotSeen) {
                        // Multiple dots in number: report error.
                        tokens.push_back({TokenType::ERROR,
                            std::string("Malformed number literal (multiple dots): ") + currentLexeme + c});
                        pos++; // Consume the extra dot.
                        // Finalize the current number token.
                        tokens.push_back({TokenType::NUMBER, currentLexeme});
                        state = State::START;
                    } else {
                        dotSeen = true;
                        currentLexeme.push_back(c);
                        pos++;
                    }
                } else {
                    // End of number literal.
                    tokens.push_back({TokenType::NUMBER, currentLexeme});
                    state = State::START;
                }
                break;

            case State::STRING:
                if (c == currentQuote) {
                    // End of string literal.
                    tokens.push_back({TokenType::STRING, currentLexeme});
                    pos++;  // Consume the closing quote.
                    state = State::START;
                } else {
                    currentLexeme.push_back(c);
                    pos++;
                }
                break;
        }
    }

    // Flush any unfinished lexemes.
    if (state == State::IDENTIFIER) {
        tokens.push_back(keywordOrIdentifier(currentLexeme));
    }
    else if (state == State::NUMBER) {
        tokens.push_back({TokenType::NUMBER, currentLexeme});
    }
    else if (state == State::STRING) {
        // Unterminated string literal error.
        tokens.push_back({TokenType::ERROR, std::string("Unterminated string literal: ") + currentLexeme});
    }

    // Append the END token.
    tokens.push_back({TokenType::END, ""});
    return tokens;
}
