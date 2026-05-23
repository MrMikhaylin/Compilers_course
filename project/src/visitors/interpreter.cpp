#include "visitors/interpreter.hpp"
#include <iostream>
#include <stdexcept>

Interpreter::Interpreter() {
    enterScope();
}

void Interpreter::enterScope() {
    scopeStack.push(std::unordered_map<std::string, int>());
}

void Interpreter::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop();
    }
}

void Interpreter::declareVariable(const std::string& name, int value) {
    auto& currentScope = scopeStack.top();
    if (currentScope.find(name) != currentScope.end()) {
        return;
    }
    currentScope[name] = value;
}

void Interpreter::setVariable(const std::string& name, int value) {
    std::vector<std::unordered_map<std::string, int>> scopes;
    while (!scopeStack.empty()) {
        scopes.push_back(scopeStack.top());
        scopeStack.pop();
    }
    
    bool found = false;
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            it->second = value;
            found = true;
            break;
        }
    }
    
    for (int i = scopes.size() - 1; i >= 0; --i) {
        scopeStack.push(scopes[i]);
    }
    
    if (!found) {
        throw std::runtime_error("Variable not declared: " + name);
    }
}

int Interpreter::getVariable(const std::string& name) {
    std::vector<std::unordered_map<std::string, int>> scopes;
    while (!scopeStack.empty()) {
        scopes.push_back(scopeStack.top());
        scopeStack.pop();
    }
    
    int value = 0;
    bool found = false;
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            value = it->second;
            found = true;
            break;
        }
    }
    
    for (int i = scopes.size() - 1; i >= 0; --i) {
        scopeStack.push(scopes[i]);
    }
    
    if (!found) {
        throw std::runtime_error("Variable not declared: " + name);
    }
    
    return value;
}

void Interpreter::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void Interpreter::visit(NumberLiteral& node) {}

void Interpreter::visit(Variable& node) {}

void Interpreter::visit(BinaryOp& node) {}

void Interpreter::visit(VarDecl& node) {
    declareVariable(node.name, 0);
}

void Interpreter::visit(Assignment& node) {
    int value = evaluate(node.value.get());
    setVariable(node.name, value);
}

void Interpreter::visit(PrintStmt& node) {
    int value = evaluate(node.expr.get());
    std::cout << value << std::endl;
}

void Interpreter::visit(IfStmt& node) {
    int conditionValue = evaluate(node.condition.get());
    
    if (conditionValue != 0) {
        for (auto& stmt : node.thenBranch) {
            stmt->accept(*this);
        }
    } else {
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
        }
    }
}

void Interpreter::visit(WhileStmt& node) {
    while (evaluate(node.condition.get()) != 0) {
        for (auto& stmt : node.body) {
            stmt->accept(*this);
        }
    }
}

void Interpreter::visit(BlockStatement& node) {
    std::unordered_map<std::string, int> shadowedVars;
    
    for (auto& stmt : node.statements) {
        if (auto* decl = dynamic_cast<VarDecl*>(stmt.get())) {
            try {
                int outerValue = getVariable(decl->name);
                shadowedVars[decl->name] = outerValue;
            } catch (...) {}
        }
    }
    
    enterScope();
    
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    
    exitScope();
    
    for (const auto& [name, value] : shadowedVars) {
        setVariable(name, value);
    }
}

int Interpreter::evaluate(Expression* expr) {
    class Evaluator : public ASTVisitor {
    public:
        int result;
        Interpreter* interpreter;
        
        void visit(NumberLiteral& node) override {
            result = node.value;
        }
        
        void visit(Variable& node) override {
            result = interpreter->getVariable(node.name);
        }
        
        void visit(BinaryOp& node) override {
            Evaluator leftEval;
            leftEval.interpreter = interpreter;
            node.left->accept(leftEval);
            int left = leftEval.result;
            
            Evaluator rightEval;
            rightEval.interpreter = interpreter;
            node.right->accept(rightEval);
            int right = rightEval.result;
            
            switch (node.op) {
                case BinOpType::PLUS: result = left + right; break;
                case BinOpType::MINUS: result = left - right; break;
                case BinOpType::MULTIPLY: result = left * right; break;
                case BinOpType::DIVIDE:
                    if (right == 0) throw std::runtime_error("Division by zero");
                    result = left / right;
                    break;
                case BinOpType::EQUALS: result = (left == right) ? 1 : 0; break;
                case BinOpType::LESS: result = (left < right) ? 1 : 0; break;
                case BinOpType::GREATER: result = (left > right) ? 1 : 0; break;
                case BinOpType::LESS_EQUAL: result = (left <= right) ? 1 : 0; break;
                case BinOpType::GREATER_EQUAL: result = (left >= right) ? 1 : 0; break;
                case BinOpType::NOT_EQUAL: result = (left != right) ? 1 : 0; break;
                default: throw std::runtime_error("Unknown operator");
            }
        }
        
        void visit(Program& node) override {
            throw std::runtime_error("Program not expected in evaluator");
        }
        
        void visit(VarDecl& node) override {
            throw std::runtime_error("VarDecl not expected in evaluator");
        }
        
        void visit(Assignment& node) override {
            throw std::runtime_error("Assignment not expected in evaluator");
        }
        
        void visit(PrintStmt& node) override {
            throw std::runtime_error("PrintStmt not expected in evaluator");
        }
        
        void visit(IfStmt& node) override {
            throw std::runtime_error("IfStmt not expected in evaluator");
        }
        
        void visit(WhileStmt& node) override {
            throw std::runtime_error("WhileStmt not expected in evaluator");
        }
        
        void visit(BlockStatement& node) override {
            throw std::runtime_error("BlockStatement not expected in evaluator");
        }
        
        void visit(FieldDecl& node) override {
            throw std::runtime_error("FieldDecl not expected in evaluator");
        }
        
        void visit(MethodDecl& node) override {
            throw std::runtime_error("MethodDecl not expected in evaluator");
        }
        
        void visit(ClassDecl& node) override {
            throw std::runtime_error("ClassDecl not expected in evaluator");
        }
        
        void visit(MethodCall& node) override {
            throw std::runtime_error("MethodCall not expected in evaluator");
        }
        
        void visit(FieldAccess& node) override {
            throw std::runtime_error("FieldAccess not expected in evaluator");
        }
        
        void visit(NewObject& node) override {
            throw std::runtime_error("NewObject not expected in evaluator");
        }
        
        void visit(ReturnStmt& node) override {
            throw std::runtime_error("ReturnStmt not expected in evaluator");
        }
        
        void visit(MethodCallStmt& node) override {
            throw std::runtime_error("MethodCallStmt not expected in evaluator");
        }
        
        void visit(FieldAssignStmt& node) override {
            throw std::runtime_error("FieldAssignStmt not expected in evaluator");
        }
        
        void visit(NewStmt& node) override {
            throw std::runtime_error("NewStmt not expected in evaluator");
        }
    };
    
    Evaluator eval;
    eval.interpreter = this;
    expr->accept(eval);
    return eval.result;
}

void Interpreter::visit(FieldDecl& node) {
    throw std::runtime_error("Классы пока не поддерживаются в интерпретаторе");
}

void Interpreter::visit(MethodDecl& node) {
    throw std::runtime_error("Методы пока не поддерживаются в интерпретаторе");
}

void Interpreter::visit(ClassDecl& node) {
    throw std::runtime_error("Классы пока не поддерживаются в интерпретаторе");
}

void Interpreter::visit(MethodCall& node) {
    throw std::runtime_error("Вызов методов пока не поддерживается в интерпретаторе");
}

void Interpreter::visit(FieldAccess& node) {
    throw std::runtime_error("Доступ к полям пока не поддерживается в интерпретаторе");
}

void Interpreter::visit(NewObject& node) {
    throw std::runtime_error("Создание объектов пока не поддерживается в интерпретаторе");
}

void Interpreter::visit(ReturnStmt& node) {
    throw std::runtime_error("Оператор return пока не поддерживается в интерпретаторе");
}

void Interpreter::visit(MethodCallStmt& node) {
    throw std::runtime_error("MethodCallStmt не поддерживается в интерпретаторе");
}

void Interpreter::visit(FieldAssignStmt& node) {
    throw std::runtime_error("FieldAssignStmt не поддерживается в интерпретаторе");
}

void Interpreter::visit(NewStmt& node) {
    throw std::runtime_error("NewStmt не поддерживается в интерпретаторе");
}