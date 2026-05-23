#pragma once

#include "ast.hpp"
#include <unordered_map>
#include <string>

class Interpreter : public ASTVisitor {
private:
    std::unordered_map<std::string, int> variables;
    
    int evaluate(Expression* expr);
    
public:
    void visit(Program& node) override;
    void visit(NumberLiteral& node) override;
    void visit(Variable& node) override;
    void visit(BinaryOp& node) override;
    void visit(VarDecl& node) override;
    void visit(Assignment& node) override;
    void visit(PrintStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;
};