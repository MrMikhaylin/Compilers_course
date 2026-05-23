#include "visitors/interpreter.hpp"
#include <iostream>
#include <stdexcept>

Interpreter::Interpreter() {
    enterScope();  // Глобальный скоуп
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
        return;  // Уже объявлена в этом скоупе - игнорируем
    }
    currentScope[name] = value;
}

void Interpreter::setVariable(const std::string& name, int value) {
    // Временно вытаскиваем все скоупы
    std::vector<std::unordered_map<std::string, int>> scopes;
    while (!scopeStack.empty()) {
        scopes.push_back(scopeStack.top());
        scopeStack.pop();
    }
    
    bool found = false;
    // Идём от глобального к локальному (обратный порядок)
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            it->second = value;
            found = true;
            break;
        }
    }
    
    // Восстанавливаем стек
    for (int i = scopes.size() - 1; i >= 0; --i) {
        scopeStack.push(scopes[i]);
    }
    
    if (!found) {
        throw std::runtime_error("Variable not declared: " + name);
    }
}

int Interpreter::getVariable(const std::string& name) {
    // Временно вытаскиваем все скоупы
    std::vector<std::unordered_map<std::string, int>> scopes;
    while (!scopeStack.empty()) {
        scopes.push_back(scopeStack.top());
        scopeStack.pop();
    }
    
    int value = 0;
    bool found = false;
    // Идём от глобального к локальному (обратный порядок)
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            value = it->second;
            found = true;
            break;
        }
    }
    
    // Восстанавливаем стек
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
    enterScope();
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    exitScope();
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
        
        void visit(Program&) override {}
        void visit(VarDecl&) override {}
        void visit(Assignment&) override {}
        void visit(PrintStmt&) override {}
        void visit(IfStmt&) override {}
        void visit(WhileStmt&) override {}
        void visit(BlockStatement&) override {}
    };
    
    Evaluator eval;
    eval.interpreter = this;
    expr->accept(eval);
    return eval.result;
}