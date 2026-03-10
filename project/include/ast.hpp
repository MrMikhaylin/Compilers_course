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

// Числовой литерал (например, 42)
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

// Бинарная операция (например, x + 1, x == 0)
class BinaryOp : public Expression {
public:
    std::string op;      // "+", "-", "*", "/", "=="
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    
    BinaryOp(const std::string& o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r);
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// 

// Объявление переменной: declare x: int;
class VarDecl : public Statement {
public:
    std::string name;
    std::string type;    // пока только "int"
    
    VarDecl(const std::string& n, const std::string& t = "int");
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Присваивание: x = 42;
class Assignment : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> value;
    
    Assignment(const std::string& n, std::unique_ptr<Expression> v);
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Печать: print(x);
class PrintStmt : public Statement {
public:
    std::unique_ptr<Expression> expr;
    
    PrintStmt(std::unique_ptr<Expression> e);
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Условный оператор: if (x == 0) { ... } else { ... }
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

// Программа (корневой узел) - список инструкций
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    Program() = default;
    
    void addStatement(std::unique_ptr<Statement> stmt);
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

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
};

class PrintVisitor : public ASTVisitor {
private:
    int indent = 0;
    void printIndent();
    
public:
    void visit(Program& node) override;
    void visit(NumberLiteral& node) override;
    void visit(Variable& node) override;
    void visit(BinaryOp& node) override;
    void visit(VarDecl& node) override;
    void visit(Assignment& node) override;
    void visit(PrintStmt& node) override;
    void visit(IfStmt& node) override;
};