#include "ast.hpp"
#include <stdexcept>

std::string binOpTypeToString(BinOpType op) {
    switch (op) {
        case BinOpType::PLUS: return "+";
        case BinOpType::MINUS: return "-";
        case BinOpType::MULTIPLY: return "*";
        case BinOpType::DIVIDE: return "/";
        case BinOpType::EQUALS: return "==";
        case BinOpType::LESS: return "<";
        case BinOpType::GREATER: return ">";
        case BinOpType::LESS_EQUAL: return "<=";
        case BinOpType::GREATER_EQUAL: return ">=";
        case BinOpType::NOT_EQUAL: return "!=";
        default: return "unknown";
    }
}

BinOpType stringToBinOpType(const std::string& op) {
    if (op == "+") return BinOpType::PLUS;
    if (op == "-") return BinOpType::MINUS;
    if (op == "*") return BinOpType::MULTIPLY;
    if (op == "/") return BinOpType::DIVIDE;
    if (op == "==") return BinOpType::EQUALS;
    if (op == "<") return BinOpType::LESS;
    if (op == ">") return BinOpType::GREATER;
    if (op == "<=") return BinOpType::LESS_EQUAL;
    if (op == ">=") return BinOpType::GREATER_EQUAL;
    if (op == "!=") return BinOpType::NOT_EQUAL;
    throw std::runtime_error("Unknown operator: " + op);
}

// --- NumberLiteral ---
NumberLiteral::NumberLiteral(int val) : value(val) {}

void NumberLiteral::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- Variable ---
Variable::Variable(const std::string& n) : name(n) {}

void Variable::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- BinaryOp ---
BinaryOp::BinaryOp(BinOpType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : op(o), left(std::move(l)), right(std::move(r)) {}

void BinaryOp::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- VarDecl ---
VarDecl::VarDecl(const std::string& n, const std::string& t) : name(n), type(t) {}

void VarDecl::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- Assignment ---
Assignment::Assignment(const std::string& n, std::unique_ptr<Expression> v) 
    : name(n), value(std::move(v)) {}

void Assignment::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- PrintStmt ---
PrintStmt::PrintStmt(std::unique_ptr<Expression> e) : expr(std::move(e)) {}

void PrintStmt::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- IfStmt ---
IfStmt::IfStmt(std::unique_ptr<Expression> cond, 
               std::vector<std::unique_ptr<Statement>> thenStmts,
               std::vector<std::unique_ptr<Statement>> elseStmts)
    : condition(std::move(cond)), thenBranch(std::move(thenStmts)), elseBranch(std::move(elseStmts)) {}

void IfStmt::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

// --- WhileStmt ---
WhileStmt::WhileStmt(std::unique_ptr<Expression> cond,
                     std::vector<std::unique_ptr<Statement>> bodyStmts)
    : condition(std::move(cond)), body(std::move(bodyStmts)) {}

void WhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// --- Program ---
void Program::addStatement(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));
}

void Program::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}