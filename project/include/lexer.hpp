#pragma once

#include "token.hpp"
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

class Lexer {
private:
    std::string input;
    size_t pos;
    int line;
    int column;
    
    std::unordered_map<std::string, TokenType> keywords;
    
    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    Token readIdentifier();
    Token readNumber();
    
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();
};