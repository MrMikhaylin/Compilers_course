#include "visitors/interpreter.hpp"
#include <iostream>
#include <stdexcept>

void Interpreter::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void Interpreter::visit(NumberLiteral& node) {}

void Interpreter::visit(Variable& node) {}

void Interpreter::visit(BinaryOp& node) {}

void Interpreter::visit(VarDecl& node) {
    if (variables.find(node.name) == variables.end()) {
        variables[node.name] = 0;
    }
}

void Interpreter::visit(Assignment& node) {
    int value = evaluate(node.value.get());
    variables[node.name] = value;
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

int Interpreter::evaluate(Expression* expr) {
    class Evaluator : public ASTVisitor {
    public:
        int result;
        std::unordered_map<std::string, int>* vars;
        
        void visit(NumberLiteral& node) override {
            result = node.value;
        }
        
        void visit(Variable& node) override {
            auto it = vars->find(node.name);
            if (it == vars->end()) {
                throw std::runtime_error("Variable not declared: " + node.name);
            }
            result = it->second;
        }
        
        void visit(BinaryOp& node) override {
            Evaluator leftEval;
            leftEval.vars = vars;
            node.left->accept(leftEval);
            int left = leftEval.result;
            
            Evaluator rightEval;
            rightEval.vars = vars;
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
    };
    
    Evaluator eval;
    eval.vars = &variables;
    expr->accept(eval);
    return eval.result;
}