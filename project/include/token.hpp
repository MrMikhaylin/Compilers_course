#pragma once

#include <string>
#include <iostream>
#include <vector>

enum class TokenType {
    // Ключевые слова
    DECLARE,    // declare
    INT,        // int
    IF,         // if
    ELSE,       // else
    PRINT,      // print
    
    // Идентификаторы и литералы
    IDENTIFIER,
    NUMBER,
    
    // Операторы
    ASSIGN,     // =
    EQUALS,     // ==
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
    
    std::string toString() const {
        std::string typeStr;
        switch(type) {
            case TokenType::DECLARE: typeStr = "DECLARE"; break;
            case TokenType::INT: typeStr = "INT"; break;
            case TokenType::IF: typeStr = "IF"; break;
            case TokenType::ELSE: typeStr = "ELSE"; break;
            case TokenType::PRINT: typeStr = "PRINT"; break;
            case TokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
            case TokenType::NUMBER: typeStr = "NUMBER"; break;
            case TokenType::ASSIGN: typeStr = "ASSIGN"; break;
            case TokenType::EQUALS: typeStr = "EQUALS"; break;
            case TokenType::PLUS: typeStr = "PLUS"; break;
            case TokenType::MINUS: typeStr = "MINUS"; break;
            case TokenType::MULTIPLY: typeStr = "MULTIPLY"; break;
            case TokenType::DIVIDE: typeStr = "DIVIDE"; break;
            case TokenType::SEMICOLON: typeStr = "SEMICOLON"; break;
            case TokenType::COLON: typeStr = "COLON"; break;
            case TokenType::LPAREN: typeStr = "LPAREN"; break;
            case TokenType::RPAREN: typeStr = "RPAREN"; break;
            case TokenType::LBRACE: typeStr = "LBRACE"; break;
            case TokenType::RBRACE: typeStr = "RBRACE"; break;
            case TokenType::END: typeStr = "END"; break;
            default: typeStr = "UNKNOWN";
        }
        return typeStr + "('" + value + "') at " + std::to_string(line) + ":" + std::to_string(column);
    }
};