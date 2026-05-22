#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTVisitor;

// Базовый класс для всех узлов AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    virtual std::string toString() const = 0;
};

// Базовый класс для выражений
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

// Базовый класс для инструкций
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

// Типы операций (enum вместо string)
enum class BinOpType {
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    EQUALS,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    NOT_EQUAL
};

// Числовой литерал
class NumberLiteral : public Expression {
public:
    int value;
    NumberLiteral(int val);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Переменная
class Variable : public Expression {
public:
    std::string name;
    Variable(const std::string& n);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Бинарная операция
class BinaryOp : public Expression {
public:
    BinOpType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinaryOp(BinOpType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Объявление переменной
class VarDecl : public Statement {
public:
    std::string name;
    std::string type;
    VarDecl(const std::string& n, const std::string& t = "int");
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Присваивание
class Assignment : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> value;
    Assignment(const std::string& n, std::unique_ptr<Expression> v);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Печать
class PrintStmt : public Statement {
public:
    std::unique_ptr<Expression> expr;
    PrintStmt(std::unique_ptr<Expression> e);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Условный оператор
class IfStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseBranch;
    IfStmt(std::unique_ptr<Expression> cond, 
           std::vector<std::unique_ptr<Statement>> thenStmts,
           std::vector<std::unique_ptr<Statement>> elseStmts = {});
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Цикл while
class WhileStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
    WhileStmt(std::unique_ptr<Expression> cond,
              std::vector<std::unique_ptr<Statement>> bodyStmts);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Блок инструкций { ... }
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    BlockStatement() = default;
    void addStatement(std::unique_ptr<Statement> stmt);
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Программа
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    Program() = default;
    void addStatement(std::unique_ptr<Statement> stmt);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Базовый класс визитора
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(Program& node) = 0;
    virtual void visit(NumberLiteral& node) = 0;
    virtual void visit(Variable& node) = 0;
    virtual void visit(BinaryOp& node) = 0;
    virtual void visit(VarDecl& node) = 0;
    virtual void visit(Assignment& node) = 0;
    virtual void visit(PrintStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(BlockStatement& node) = 0;
};

// Вспомогательная функция для преобразования BinOpType в строку
std::string binOpTypeToString(BinOpType op);
BinOpType stringToBinOpType(const std::string& op);