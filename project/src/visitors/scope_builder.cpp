#include "visitors/scope_builder.hpp"
#include <iostream>

ScopeBuilder::ScopeBuilder() 
    : globalScope(std::make_unique<Scope>(nullptr, "global")), 
      currentScope(globalScope.get()) {}

void ScopeBuilder::enterScope(const std::string& name) {
    scopeStack.push(currentScope);
    currentScope = currentScope->createChild(name);
}

void ScopeBuilder::exitScope() {
    if (!scopeStack.empty()) {
        currentScope = scopeStack.top();
        scopeStack.pop();
    }
}

void ScopeBuilder::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void ScopeBuilder::visit(NumberLiteral& /*node*/) {
    // Nothing to do
}

void ScopeBuilder::visit(Variable& node) {
    Symbol* sym = currentScope->lookup(node.name);
    if (!sym) {
        errors.push_back("Ошибка: переменная '" + node.name + "' не объявлена");
    } else if (sym->kind != SymbolKind::VARIABLE) {
        errors.push_back("Ошибка: '" + node.name + "' не является переменной");
    }
}

void ScopeBuilder::visit(BinaryOp& node) {
    node.left->accept(*this);
    node.right->accept(*this);
}

void ScopeBuilder::visit(VarDecl& node) {
    if (currentScope->lookupLocal(node.name)) {
        errors.push_back("Ошибка: переменная '" + node.name + "' уже объявлена в этом скоупе");
        return;
    }
    
    auto varSym = std::make_unique<VariableSymbol>(node.name, node.type);
    currentScope->addSymbol(std::move(varSym));
}

void ScopeBuilder::visit(Assignment& node) {
    Symbol* sym = currentScope->lookup(node.name);
    if (!sym) {
        errors.push_back("Ошибка: переменная '" + node.name + "' не объявлена");
    } else if (sym->kind != SymbolKind::VARIABLE) {
        errors.push_back("Ошибка: '" + node.name + "' не является переменной");
    }
    
    node.value->accept(*this);
}

void ScopeBuilder::visit(PrintStmt& node) {
    node.expr->accept(*this);
}

void ScopeBuilder::visit(IfStmt& node) {
    node.condition->accept(*this);
    
    enterScope("if-then");
    for (auto& stmt : node.thenBranch) {
        stmt->accept(*this);
    }
    exitScope();
    
    if (!node.elseBranch.empty()) {
        enterScope("if-else");
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
        }
        exitScope();
    }
}

void ScopeBuilder::visit(WhileStmt& node) {
    node.condition->accept(*this);
    
    enterScope("while-body");
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    exitScope();
}

void ScopeBuilder::visit(BlockStatement& node) {
    enterScope("block");
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    exitScope();
}

void ScopeBuilder::reportErrors() {
    if (errors.empty()) {
        std::cout << "Ошибок не обнаружено!" << std::endl;
    } else {
        std::cout << "Найдено ошибок: " << errors.size() << std::endl;
        for (const auto& err : errors) {
            std::cout << "  " << err << std::endl;
        }
    }
}