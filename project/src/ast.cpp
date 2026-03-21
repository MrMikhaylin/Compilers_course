#include "ast.hpp"
#include <sstream>
#include <iostream>

// --- NumberLiteral ---
NumberLiteral::NumberLiteral(int val) : value(val) {}

void NumberLiteral::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string NumberLiteral::toString() const {
    return "Number(" + std::to_string(value) + ")";
}

// --- Variable ---
Variable::Variable(const std::string& n) : name(n) {}

void Variable::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string Variable::toString() const {
    return "Variable(" + name + ")";
}

// --- BinaryOp ---
BinaryOp::BinaryOp(const std::string& o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : op(o), left(std::move(l)), right(std::move(r)) {}

void BinaryOp::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string BinaryOp::toString() const {
    return "BinaryOp(" + op + ", " + left->toString() + ", " + right->toString() + ")";
}

// --- VarDecl ---
VarDecl::VarDecl(const std::string& n, const std::string& t) : name(n), type(t) {}

void VarDecl::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string VarDecl::toString() const {
    return "VarDecl(" + name + ", " + type + ")";
}

// --- Assignment ---
Assignment::Assignment(const std::string& n, std::unique_ptr<Expression> v) 
    : name(n), value(std::move(v)) {}

void Assignment::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string Assignment::toString() const {
    return "Assignment(" + name + ", " + value->toString() + ")";
}

// --- PrintStmt ---
PrintStmt::PrintStmt(std::unique_ptr<Expression> e) : expr(std::move(e)) {}

void PrintStmt::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string PrintStmt::toString() const {
    return "Print(" + expr->toString() + ")";
}

// --- IfStmt ---
IfStmt::IfStmt(std::unique_ptr<Expression> cond, 
               std::vector<std::unique_ptr<Statement>> thenStmts,
               std::vector<std::unique_ptr<Statement>> elseStmts)
    : condition(std::move(cond)), thenBranch(std::move(thenStmts)), elseBranch(std::move(elseStmts)) {}

void IfStmt::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string IfStmt::toString() const {
    std::string result = "If(" + condition->toString() + ", [";
    for (const auto& stmt : thenBranch) {
        result += stmt->toString() + ", ";
    }
    result += "], [";
    for (const auto& stmt : elseBranch) {
        result += stmt->toString() + ", ";
    }
    result += "])";
    return result;
}

// --- Program ---
void Program::addStatement(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));
}

void Program::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string Program::toString() const {
    std::string result = "Program([";
    for (const auto& stmt : statements) {
        result += stmt->toString() + ", ";
    }
    result += "])";
    return result;
}