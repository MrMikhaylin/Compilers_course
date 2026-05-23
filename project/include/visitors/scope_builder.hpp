#pragma once

#include "ast.hpp"
#include "scope.hpp"
#include <stack>
#include <vector>
#include <string>

class ScopeBuilder : public ASTVisitor {
private:
    std::unique_ptr<Scope> globalScope;
    Scope* currentScope;
    std::stack<Scope*> scopeStack;
    std::vector<std::string> errors;
    
    void enterScope(const std::string& name);
    void exitScope();
    
public:
    ScopeBuilder();
    
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
    
    Scope* getGlobalScope() const { return globalScope.get(); }
    void reportErrors();
};