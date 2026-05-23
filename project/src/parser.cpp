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
    
    Token currentToken = peek();
    std::string error = "Ошибка в " + std::to_string(currentToken.line) + ":" + 
                        std::to_string(currentToken.column) + " - " + message;
    throw ParseError(error);
}

void Parser::synchronize() {
    advance();
    
    while (!check(TokenType::END)) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::DECLARE:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::IDENTIFIER:
                return;
            default:
                advance();
        }
    }
}

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
    if (match(TokenType::WHILE)) {
        return parseWhileStmt();
    }
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    if (match(TokenType::CLASS)) {
        return parseClassDecl();
    }
    if (match(TokenType::RETURN)) {
        return parseReturnStmt();
    }
    if (match(TokenType::IDENTIFIER)) {
        std::string identifier = previous().value;
        
        if (match(TokenType::ASSIGN)) {
            auto value = parseExpression();
            consume(TokenType::SEMICOLON, "Ожидается ';' после присваивания");
            return std::make_unique<Assignment>(identifier, std::move(value));
        } else if (match(TokenType::DOT)) {
            if (check(TokenType::IDENTIFIER)) {
                size_t savedPos = current;
                Token temp = advance();
                bool isMethod = check(TokenType::LPAREN);
                current = savedPos;
                
                if (isMethod) {
                    auto expr = parseMethodCall(identifier);
                    consume(TokenType::SEMICOLON, "Ожидается ';' после вызова метода");
                    return std::make_unique<MethodCallStmt>(std::move(expr));
                } else {
                    auto expr = parseFieldAccess(identifier);
                    if (match(TokenType::ASSIGN)) {
                        auto value = parseExpression();
                        consume(TokenType::SEMICOLON, "Ожидается ';' после присваивания");
                        return std::make_unique<FieldAssignStmt>(std::move(expr), std::move(value));
                    } else {
                        consume(TokenType::SEMICOLON, "Ожидается ';'");
                        return nullptr;
                    }
                }
            }
        } else {
            Token unexpected = peek();
            std::string error = "Неожиданный токен после идентификатора: " + unexpected.toString();
            throw ParseError(error);
        }
    }
    if (match(TokenType::NEW)) {
        auto expr = parseNewObject();
        consume(TokenType::SEMICOLON, "Ожидается ';' после new");
        return std::make_unique<NewStmt>(std::move(expr));
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
    
    Token typeToken = consume(TokenType::INT, "Ожидается тип переменной (int)");
    
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

std::unique_ptr<Statement> Parser::parseWhileStmt() {
    consume(TokenType::LPAREN, "Ожидается '(' после while");
    
    auto condition = parseExpression();
    
    consume(TokenType::RPAREN, "Ожидается ')' после условия в while");
    
    std::vector<std::unique_ptr<Statement>> body;
    
    if (match(TokenType::LBRACE)) {
        auto block = parseBlock();
        BlockStatement* blockStmt = dynamic_cast<BlockStatement*>(block.get());
        if (blockStmt) {
            body = std::move(blockStmt->statements);
        }
    } else {
        auto stmt = parseStatement();
        if (stmt) {
            body.push_back(std::move(stmt));
        }
    }
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
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

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseEquality();
    
    while (match({TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
        BinOpType op;
        switch (previous().type) {
            case TokenType::LESS: op = BinOpType::LESS; break;
            case TokenType::GREATER: op = BinOpType::GREATER; break;
            case TokenType::LESS_EQUAL: op = BinOpType::LESS_EQUAL; break;
            case TokenType::GREATER_EQUAL: op = BinOpType::GREATER_EQUAL; break;
            default: throw ParseError("Unknown comparison operator");
        }
        auto right = parseEquality();
        expr = std::make_unique<BinaryOp>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseAdditive();
    
    while (match({TokenType::EQUALS, TokenType::NOT_EQUAL})) {
        BinOpType op = (previous().type == TokenType::EQUALS) ? BinOpType::EQUALS : BinOpType::NOT_EQUAL;
        auto right = parseAdditive();
        expr = std::make_unique<BinaryOp>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        BinOpType op = (previous().type == TokenType::PLUS) ? BinOpType::PLUS : BinOpType::MINUS;
        auto right = parseMultiplicative();
        expr = std::make_unique<BinaryOp>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parseMultiplicative() {
    auto expr = parsePrimary();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE})) {
        BinOpType op = (previous().type == TokenType::MULTIPLY) ? BinOpType::MULTIPLY : BinOpType::DIVIDE;
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

std::unique_ptr<Statement> Parser::parseClassDecl() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Ожидается имя класса");
    consume(TokenType::LBRACE, "Ожидается '{' после имени класса");
    
    auto classDecl = std::make_unique<ClassDecl>(nameToken.value);
    
    while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
        if (match(TokenType::DECLARE)) {
            Token fieldName = consume(TokenType::IDENTIFIER, "Ожидается имя поля");
            consume(TokenType::COLON, "Ожидается ':' после имени поля");
            Token fieldType = consume(TokenType::INT, "Ожидается тип поля (int)");
            consume(TokenType::SEMICOLON, "Ожидается ';' после объявления поля");
            classDecl->addField(std::make_unique<FieldDecl>(fieldName.value, fieldType.value));
        } else if (match(TokenType::DEF)) {
            Token methodName = consume(TokenType::IDENTIFIER, "Ожидается имя метода");
            consume(TokenType::LPAREN, "Ожидается '(' после имени метода");
            
            auto methodDecl = std::make_unique<MethodDecl>(methodName.value);
            
            if (!check(TokenType::RPAREN)) {
                do {
                    Token paramName = consume(TokenType::IDENTIFIER, "Ожидается имя параметра");
                    consume(TokenType::COLON, "Ожидается ':' после имени параметра");
                    consume(TokenType::INT, "Ожидается тип параметра (int)");
                    methodDecl->addParameter(paramName.value);
                } while (match(TokenType::COMMA));
            }
            
            consume(TokenType::RPAREN, "Ожидается ')' после параметров");
            consume(TokenType::COLON, "Ожидается ':' после параметров");
            Token returnType = consume(TokenType::INT, "Ожидается тип возвращаемого значения (int)");
            methodDecl->returnType = returnType.value;
            
            consume(TokenType::LBRACE, "Ожидается '{' перед телом метода");
            while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
                auto stmt = parseStatement();
                if (stmt) {
                    methodDecl->addStatement(std::move(stmt));
                }
            }
            consume(TokenType::RBRACE, "Ожидается '}' после тела метода");
            
            classDecl->addMethod(std::move(methodDecl));
        } else {
            Token unexpected = peek();
            std::string error = "Неожиданный токен в теле класса: " + unexpected.toString();
            throw ParseError(error);
        }
    }
    
    consume(TokenType::RBRACE, "Ожидается '}' после тела класса");
    return classDecl;
}

std::unique_ptr<Statement> Parser::parseMethodDecl() {
    throw ParseError("Методы вне классов не поддерживаются");
}

std::unique_ptr<MethodCall> Parser::parseMethodCall(const std::string& object) {
    Token methodToken = consume(TokenType::IDENTIFIER, "Ожидается имя метода");
    consume(TokenType::LPAREN, "Ожидается '(' после имени метода");
    
    auto methodCall = std::make_unique<MethodCall>(object, methodToken.value);
    
    if (!check(TokenType::RPAREN)) {
        do {
            auto arg = parseExpression();
            methodCall->addArgument(std::move(arg));
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RPAREN, "Ожидается ')' после аргументов");
    return methodCall;
}

std::unique_ptr<FieldAccess> Parser::parseFieldAccess(const std::string& object) {
    Token fieldToken = consume(TokenType::IDENTIFIER, "Ожидается имя поля");
    return std::make_unique<FieldAccess>(object, fieldToken.value);
}

std::unique_ptr<Statement> Parser::parseReturnStmt() {
    if (check(TokenType::SEMICOLON)) {
        advance();
        return std::make_unique<ReturnStmt>(nullptr);
    }
    
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Ожидается ';' после return");
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<NewObject> Parser::parseNewObject() {
    Token className = consume(TokenType::IDENTIFIER, "Ожидается имя класса");
    consume(TokenType::LPAREN, "Ожидается '(' после new");
    consume(TokenType::RPAREN, "Ожидается ')' после new");
    return std::make_unique<NewObject>(className.value);
}