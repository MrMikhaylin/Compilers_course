#include "token.hpp"

const std::unordered_map<TokenType, std::string> TOKEN_TYPE_NAMES = {
    {TokenType::DECLARE, "DECLARE"},
    {TokenType::INT, "INT"},
    {TokenType::IF, "IF"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::PRINT, "PRINT"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::NUMBER, "NUMBER"},
    {TokenType::ASSIGN, "ASSIGN"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::LESS, "LESS"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
    {TokenType::NOT_EQUAL, "NOT_EQUAL"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::MULTIPLY, "MULTIPLY"},
    {TokenType::DIVIDE, "DIVIDE"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COLON, "COLON"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::LBRACE, "LBRACE"},
    {TokenType::RBRACE, "RBRACE"},
    {TokenType::END, "END"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

std::string Token::toString() const {
    auto it = TOKEN_TYPE_NAMES.find(type);
    std::string typeStr = (it != TOKEN_TYPE_NAMES.end()) ? it->second : "UNKNOWN";
    return typeStr + "('" + value + "') at " + std::to_string(line) + ":" + std::to_string(column);
}