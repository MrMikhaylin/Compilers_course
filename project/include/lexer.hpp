#pragma once

#include "token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
private:
    std::string input;
    size_t pos;
    int line;
    int column;
    
    static const std::unordered_map<std::string, TokenType> KEYWORDS;
    
    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    Token readIdentifier();
    Token readNumber();
    
public:
    explicit Lexer(const std::string& input);
    std::vector<Token> tokenize();
};