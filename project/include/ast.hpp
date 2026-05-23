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

// Блок инструкций
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    BlockStatement() = default;
    void addStatement(std::unique_ptr<Statement> stmt);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Поле класса
class FieldDecl : public Statement {
public:
    std::string name;
    std::string type;
    FieldDecl(const std::string& n, const std::string& t = "int");
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Метод класса
class MethodDecl : public Statement {
public:
    std::string name;
    std::string returnType;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<Statement>> body;
    
    MethodDecl(const std::string& n, const std::string& ret = "int");
    void addParameter(const std::string& param);
    void addStatement(std::unique_ptr<Statement> stmt);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Объявление класса
class ClassDecl : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<FieldDecl>> fields;
    std::vector<std::unique_ptr<MethodDecl>> methods;
    
    ClassDecl(const std::string& n);
    void addField(std::unique_ptr<FieldDecl> field);
    void addMethod(std::unique_ptr<MethodDecl> method);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Вызов метода
class MethodCall : public Expression {
public:
    std::string object;
    std::string method;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    MethodCall(const std::string& obj, const std::string& meth);
    void addArgument(std::unique_ptr<Expression> arg);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Доступ к полю
class FieldAccess : public Expression {
public:
    std::string object;
    std::string field;
    
    FieldAccess(const std::string& obj, const std::string& fld);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Создание объекта (new)
class NewObject : public Expression {
public:
    std::string className;
    
    NewObject(const std::string& name);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Оператор return
class ReturnStmt : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    ReturnStmt(std::unique_ptr<Expression> val = nullptr);
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

// Вызов метода как инструкция (statement)
class MethodCallStmt : public Statement {
public:
    std::unique_ptr<MethodCall> call;
    MethodCallStmt(std::unique_ptr<MethodCall> c);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Присваивание поля как инструкция
class FieldAssignStmt : public Statement {
public:
    std::unique_ptr<FieldAccess> field;
    std::unique_ptr<Expression> value;
    FieldAssignStmt(std::unique_ptr<FieldAccess> f, std::unique_ptr<Expression> v);
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// Создание объекта как инструкция
class NewStmt : public Statement {
public:
    std::unique_ptr<NewObject> newObj;
    NewStmt(std::unique_ptr<NewObject> n);
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
    
    virtual void visit(FieldDecl& node) = 0;
    virtual void visit(MethodDecl& node) = 0;
    virtual void visit(ClassDecl& node) = 0;
    virtual void visit(MethodCall& node) = 0;
    virtual void visit(FieldAccess& node) = 0;
    virtual void visit(NewObject& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;

    virtual void visit(MethodCallStmt& node) = 0;
    virtual void visit(FieldAssignStmt& node) = 0;
    virtual void visit(NewStmt& node) = 0;
};

// Вспомогательные функции
std::string binOpTypeToString(BinOpType op);
BinOpType stringToBinOpType(const std::string& op);