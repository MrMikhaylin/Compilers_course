#include "parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

Token Parser::peek() const {
    if (current >= tokens.size()) return tokens.back();
    return tokens[current];
}

Token Parser::previous() const {
    if (current == 0) return tokens[0];
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!check(TokenType::END)) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (current >= tokens.size()) return false;
    return tokens[current].type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (match(type)) return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    Token current = peek();
    std::string error = "Ошибка в " + std::to_string(current.line) + ":" + 
                        std::to_string(current.column) + " - " + message;
    throw ParseError(error);
}

void Parser::synchronize() {
    advance();
    
    while (!check(TokenType::END)) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::DECLARE:
            case TokenType::IF:
            case TokenType::PRINT:
            case TokenType::IDENTIFIER:
                return;
            default:
                advance();
        }
    }
}

// --- Методы парсинга ---

std::unique_ptr<Program> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    
    while (!check(TokenType::END)) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->addStatement(std::move(stmt));
            }
        } catch (const ParseError& e) {
            std::cerr << "Ошибка парсинга: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (match(TokenType::DECLARE)) {
        return parseVarDecl();
    }
    if (match(TokenType::PRINT)) {
        return parsePrintStmt();
    }
    if (match(TokenType::IF)) {
        return parseIfStmt();
    }
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    if (match(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }
    
    if (!check(TokenType::END)) {
        Token unexpected = peek();
        std::string error = "Неожиданный токен: " + unexpected.toString();
        throw ParseError(error);
    }
    
    return nullptr;
}

std::unique_ptr<Statement> Parser::parseVarDecl() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Ожидается имя переменной после 'declare'");
    consume(TokenType::COLON, "Ожидается ':' после имени переменной");
    
    Token typeToken;
    if (match(TokenType::INT)) {
        typeToken = previous();
    } else {
        typeToken = consume(TokenType::IDENTIFIER, "Ожидается тип переменной (int)");
    }
    
    consume(TokenType::SEMICOLON, "Ожидается ';' после объявления переменной");
    
    return std::make_unique<VarDecl>(nameToken.value, typeToken.value);
}

std::unique_ptr<Statement> Parser::parseAssignment() {
    Token nameToken = previous();
    
    consume(TokenType::ASSIGN, "Ожидается '=' в присваивании");
    
    auto value = parseExpression();
    
    consume(TokenType::SEMICOLON, "Ожидается ';' после присваивания");
    
    return std::make_unique<Assignment>(nameToken.value, std::move(value));
}

std::unique_ptr<Statement> Parser::parsePrintStmt() {
    consume(TokenType::LPAREN, "Ожидается '(' после print");
    
    auto expr = parseExpression();
    
    consume(TokenType::RPAREN, "Ожидается ')' после выражения в print");
    consume(TokenType::SEMICOLON, "Ожидается ';' после print");
    
    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Statement> Parser::parseIfStmt() {
    consume(TokenType::LPAREN, "Ожидается '(' после if");
    
    auto condition = parseExpression();
    
    consume(TokenType::RPAREN, "Ожидается ')' после условия в if");
    
    // Then branch
    std::vector<std::unique_ptr<Statement>> thenBranch;
    
    if (match(TokenType::LBRACE)) {
        auto block = parseBlock();
        BlockStatement* blockStmt = dynamic_cast<BlockStatement*>(block.get());
        if (blockStmt) {
            thenBranch = std::move(blockStmt->statements);
        }
    } else {
        auto stmt = parseStatement();
        if (stmt) {
            thenBranch.push_back(std::move(stmt));
        }
    }
    
    // Else branch
    std::vector<std::unique_ptr<Statement>> elseBranch;
    if (match(TokenType::ELSE)) {
        if (match(TokenType::LBRACE)) {
            auto block = parseBlock();
            BlockStatement* blockStmt = dynamic_cast<BlockStatement*>(block.get());
            if (blockStmt) {
                elseBranch = std::move(blockStmt->statements);
            }
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                elseBranch.push_back(std::move(stmt));
            }
        }
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::parseBlock() {
    auto block = std::make_unique<BlockStatement>();
    
    while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
        auto stmt = parseStatement();
        if (stmt) {
            block->addStatement(std::move(stmt));
        }
    }
    
    consume(TokenType::RBRACE, "Ожидается '}' после блока");
    
    return block;
}

// --- Парсинг выражений с приоритетами ---

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseAdditive();
    
    while (match(TokenType::EQUALS)) {
        std::string op = previous().value;
        auto right = parseAdditive();
        expr = std::make_unique<BinaryOp>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = previous().value;
        auto right = parseMultiplicative();
        expr = std::make_unique<BinaryOp>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseMultiplicative() {
    auto expr = parsePrimary();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE})) {
        std::string op = previous().value;
        auto right = parsePrimary();
        expr = std::make_unique<BinaryOp>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        int value = std::stoi(previous().value);
        return std::make_unique<NumberLiteral>(value);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<Variable>(previous().value);
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Ожидается ')' после выражения");
        return expr;
    }
    
    Token unexpected = peek();
    std::string error = "Ожидается выражение, получено: " + unexpected.toString();
    throw ParseError(error);
}