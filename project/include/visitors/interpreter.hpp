#pragma once

#include "ast.hpp"
#include <unordered_map>
#include <stack>
#include <string>

class Interpreter : public ASTVisitor {
private:
    // Стек скоупов для поддержки shadowing
    std::stack<std::unordered_map<std::string, int>> scopeStack;
    
    void enterScope();
    void exitScope();
    void declareVariable(const std::string& name, int value = 0);
    void setVariable(const std::string& name, int value);
    int getVariable(const std::string& name);
    
    int evaluate(Expression* expr);
    
public:
    Interpreter();
    
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