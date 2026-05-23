#include "lexer.hpp"
#include <iostream>

const std::unordered_map<std::string, TokenType> Lexer::KEYWORDS = {
    {"declare", TokenType::DECLARE},
    {"int", TokenType::INT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"print", TokenType::PRINT},
    {"while", TokenType::WHILE},
    {"class", TokenType::CLASS},
    {"def", TokenType::DEF},
    {"new", TokenType::NEW},
    {"this", TokenType::THIS},
    {"return", TokenType::RETURN}
};

Lexer::Lexer(const std::string& input) : input(input), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (pos >= input.length()) return '\0';
    return input[pos];
}

char Lexer::advance() {
    char c = input[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && pos + 1 < input.length() && input[pos + 1] == '/') {
        advance();
        advance();
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
        if (peek() == '\n') {
            advance();
        }
    }
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startCol = column;
    std::string value;
    
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }
    
    auto it = KEYWORDS.find(value);
    TokenType type = (it != KEYWORDS.end()) ? it->second : TokenType::IDENTIFIER;
    
    return Token(type, value, startLine, startCol);
}

Token Lexer::readNumber() {
    int startLine = line;
    int startCol = column;
    std::string value;
    
    while (isdigit(peek())) {
        value += advance();
    }
    
    return Token(TokenType::NUMBER, value, startLine, startCol);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (pos < input.length()) {
        skipWhitespace();
        
        if (peek() == '/' && pos + 1 < input.length() && input[pos + 1] == '/') {
            skipComment();
            continue;
        }
        
        if (pos >= input.length()) break;
        
        char c = peek();
        
        if (isalpha(c)) {
            tokens.push_back(readIdentifier());
            continue;
        }
        
        if (isdigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }
        
        int startLine = line;
        int startCol = column;
        
        switch (c) {
            case '=':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(Token(TokenType::EQUALS, "==", startLine, startCol));
                } else {
                    tokens.push_back(Token(TokenType::ASSIGN, "=", startLine, startCol));
                }
                break;
            
            case '<':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", startLine, startCol));
                } else {
                    tokens.push_back(Token(TokenType::LESS, "<", startLine, startCol));
                }
                break;
            
            case '>':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", startLine, startCol));
                } else {
                    tokens.push_back(Token(TokenType::GREATER, ">", startLine, startCol));
                }
                break;
            
            case '!':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(Token(TokenType::NOT_EQUAL, "!=", startLine, startCol));
                } else {
                    std::cerr << "Unknown character at " << line << ":" << column << ": " << c << std::endl;
                    tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), startLine, startCol));
                }
                break;
            
            case '.':
                advance();
                tokens.push_back(Token(TokenType::DOT, ".", startLine, startCol));
                break;
            
            case ',':
                advance();
                tokens.push_back(Token(TokenType::COMMA, ",", startLine, startCol));
                break;
                
            case '+': advance(); tokens.push_back(Token(TokenType::PLUS, "+", startLine, startCol)); break;
            case '-': advance(); tokens.push_back(Token(TokenType::MINUS, "-", startLine, startCol)); break;
            case '*': advance(); tokens.push_back(Token(TokenType::MULTIPLY, "*", startLine, startCol)); break;
            case '/': 
                advance();
                tokens.push_back(Token(TokenType::DIVIDE, "/", startLine, startCol));
                break;
                
            case ';': advance(); tokens.push_back(Token(TokenType::SEMICOLON, ";", startLine, startCol)); break;
            case ':': advance(); tokens.push_back(Token(TokenType::COLON, ":", startLine, startCol)); break;
            case '(': advance(); tokens.push_back(Token(TokenType::LPAREN, "(", startLine, startCol)); break;
            case ')': advance(); tokens.push_back(Token(TokenType::RPAREN, ")", startLine, startCol)); break;
            case '{': advance(); tokens.push_back(Token(TokenType::LBRACE, "{", startLine, startCol)); break;
            case '}': advance(); tokens.push_back(Token(TokenType::RBRACE, "}", startLine, startCol)); break;
                
            default:
                std::cerr << "Unknown character at " << line << ":" << column << ": " << c << std::endl;
                advance();
                tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), startLine, startCol));
                break;
        }
    }
    
    tokens.push_back(Token(TokenType::END, "", line, column));
    return tokens;
}