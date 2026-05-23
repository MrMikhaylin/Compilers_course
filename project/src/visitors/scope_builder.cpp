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

void ScopeBuilder::visit(FieldDecl& node) {
    // Поля классов добавляются в скоуп класса
    if (currentScope->lookupLocal(node.name)) {
        errors.push_back("Ошибка: поле '" + node.name + "' уже объявлено в этом классе");
        return;
    }
    
    auto varSym = std::make_unique<VariableSymbol>(node.name, node.type);
    currentScope->addSymbol(std::move(varSym));
}

void ScopeBuilder::visit(MethodDecl& node) {
    // Создаём новый скоуп для метода
    enterScope("method-" + node.name);
    
    // Добавляем параметры как локальные переменные
    for (const auto& param : node.parameters) {
        if (currentScope->lookupLocal(param)) {
            errors.push_back("Ошибка: параметр '" + param + "' уже объявлен");
            continue;
        }
        auto varSym = std::make_unique<VariableSymbol>(param, "int");
        currentScope->addSymbol(std::move(varSym));
    }
    
    // Обрабатываем тело метода
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    
    exitScope();
}

void ScopeBuilder::visit(ClassDecl& node) {
    // Создаём скоуп для класса
    enterScope("class-" + node.name);
    
    // Добавляем сам класс в родительский скоуп
    auto classSym = std::make_unique<ClassSymbol>(node.name);
    if (currentScope->getParent()) {
        currentScope->getParent()->addSymbol(std::move(classSym));
    }
    
    // Обрабатываем поля и методы внутри класса
    for (auto& field : node.fields) {
        field->accept(*this);
    }
    for (auto& method : node.methods) {
        method->accept(*this);
    }
    
    exitScope();
}

void ScopeBuilder::visit(MethodCall& node) {
    // Проверяем, существует ли объект
    Symbol* objSym = currentScope->lookup(node.object);
    if (!objSym) {
        errors.push_back("Ошибка: объект '" + node.object + "' не объявлен");
        return;
    }
    
    // Проверяем, что это класс
    if (objSym->kind != SymbolKind::CLASS) {
        errors.push_back("Ошибка: '" + node.object + "' не является классом");
    }
    
    // Проверяем аргументы
    for (auto& arg : node.arguments) {
        arg->accept(*this);
    }
}

void ScopeBuilder::visit(FieldAccess& node) {
    // Проверяем, существует ли объект
    Symbol* objSym = currentScope->lookup(node.object);
    if (!objSym) {
        errors.push_back("Ошибка: объект '" + node.object + "' не объявлен");
    }
}

void ScopeBuilder::visit(NewObject& node) {
    // Проверяем, существует ли класс
    Symbol* classSym = currentScope->lookup(node.className);
    if (!classSym) {
        errors.push_back("Ошибка: класс '" + node.className + "' не объявлен");
    } else if (classSym->kind != SymbolKind::CLASS) {
        errors.push_back("Ошибка: '" + node.className + "' не является классом");
    }
}

void ScopeBuilder::visit(ReturnStmt& node) {
    if (node.value) {
        node.value->accept(*this);
    }
}

void ScopeBuilder::visit(MethodCallStmt& node) {
    node.call->accept(*this);
}

void ScopeBuilder::visit(FieldAssignStmt& node) {
    node.field->accept(*this);
    node.value->accept(*this);
}

void ScopeBuilder::visit(NewStmt& node) {
    node.newObj->accept(*this);
}