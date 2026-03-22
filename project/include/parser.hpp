#pragma once

#include "token.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>
#include <string>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    // Вспомогательные методы
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    
    // Парсинг различных конструкций
    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseVarDecl();
    std::unique_ptr<Statement> parseAssignment();
    std::unique_ptr<Statement> parsePrintStmt();
    std::unique_ptr<Statement> parseIfStmt();
    std::unique_ptr<Statement> parseBlock();
    
    // Парсинг выражений (с приоритетами)
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseEquality();    // ==
    std::unique_ptr<Expression> parseAdditive();     // +, -
    std::unique_ptr<Expression> parseMultiplicative(); // *, /
    std::unique_ptr<Expression> parsePrimary();      // числа, переменные, ( )
    
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
    
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& msg) : std::runtime_error(msg) {}
    };
};