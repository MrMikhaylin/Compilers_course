#pragma once

#include "ast.hpp"
#include <unordered_map>
#include <string>

// Визитор для интерпретации программы
class Interpreter : public ASTVisitor {
private:
    // Таблица переменных: имя -> значение
    std::unordered_map<std::string, int> variables;
    
    // Вычисляет значение выражения
    int evaluate(Expression* expr);
    
public:
    // Методы visit для всех узлов AST
    void visit(Program& node) override;
    void visit(NumberLiteral& node) override;
    void visit(Variable& node) override;
    void visit(BinaryOp& node) override;
    void visit(VarDecl& node) override;
    void visit(Assignment& node) override;
    void visit(PrintStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(BlockStatement& node) override;
};