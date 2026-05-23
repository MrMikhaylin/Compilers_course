#pragma once

#include <string>
#include <unordered_map>

enum class TokenType {
    // Ключевые слова
    DECLARE,    // declare
    INT,        // int
    IF,         // if
    ELSE,       // else
    PRINT,      // print
    WHILE,      // while
    
    // Идентификаторы и литералы
    IDENTIFIER,
    NUMBER,
    
    // Операторы
    ASSIGN,     // =
    EQUALS,     // ==
    LESS,       // <
    GREATER,    // >
    LESS_EQUAL, // <=
    GREATER_EQUAL, // >=
    NOT_EQUAL,  // !=
    PLUS,       // +
    MINUS,      // -
    MULTIPLY,   // *
    DIVIDE,     // /
    
    // Разделители
    SEMICOLON,  // ;
    COLON,      // :
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    
    // Специальные
    END,        // Конец файла
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token() : type(TokenType::UNKNOWN), value(""), line(0), column(0) {}
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
    
    std::string toString() const;
};

extern const std::unordered_map<TokenType, std::string> TOKEN_TYPE_NAMES;