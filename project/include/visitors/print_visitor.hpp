#pragma once

#include "ast.hpp"
#include <ostream>

class PrintVisitor : public ASTVisitor {
private:
    std::ostream& out;
    int indent;
    
    void printIndent();
    
public:
    PrintVisitor(std::ostream& os = std::cout);
    
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
    
    void visit(FieldDecl& node) override;
    void visit(MethodDecl& node) override;
    void visit(ClassDecl& node) override;
    void visit(MethodCall& node) override;
    void visit(FieldAccess& node) override;
    void visit(NewObject& node) override;
    void visit(ReturnStmt& node) override;
    
    void visit(MethodCallStmt& node) override;
    void visit(FieldAssignStmt& node) override;
    void visit(NewStmt& node) override;
};