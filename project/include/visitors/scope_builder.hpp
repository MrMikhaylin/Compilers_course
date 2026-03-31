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
    void visit(BlockStatement& node) override;
    
    Scope* getGlobalScope() const { return globalScope.get(); }
    void reportErrors();
};