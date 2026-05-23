#include "ast.hpp"
#include <sstream>
#include <iostream>
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
BinaryOp::BinaryOp(BinOpType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
    : op(o), left(std::move(l)), right(std::move(r)) {}

void BinaryOp::accept(ASTVisitor& visitor) { 
    visitor.visit(*this); 
}

std::string BinaryOp::toString() const {
    return "BinaryOp(" + binOpTypeToString(op) + ", " + left->toString() + ", " + right->toString() + ")";
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

// --- WhileStmt ---
WhileStmt::WhileStmt(std::unique_ptr<Expression> cond,
                     std::vector<std::unique_ptr<Statement>> bodyStmts)
    : condition(std::move(cond)), body(std::move(bodyStmts)) {}

void WhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string WhileStmt::toString() const {
    std::string result = "While(" + condition->toString() + ", [";
    for (const auto& stmt : body) {
        result += stmt->toString() + ", ";
    }
    result += "])";
    return result;
}

// --- BlockStatement ---
void BlockStatement::addStatement(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));
}

void BlockStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

std::string BlockStatement::toString() const {
    std::string result = "Block([";
    for (const auto& stmt : statements) {
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

// --- FieldDecl ---
FieldDecl::FieldDecl(const std::string& n, const std::string& t) : name(n), type(t) {}
void FieldDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string FieldDecl::toString() const { return "Field(" + name + ": " + type + ")"; }

// --- MethodDecl ---
MethodDecl::MethodDecl(const std::string& n, const std::string& ret) : name(n), returnType(ret) {}
void MethodDecl::addParameter(const std::string& param) { parameters.push_back(param); }
void MethodDecl::addStatement(std::unique_ptr<Statement> stmt) { body.push_back(std::move(stmt)); }
void MethodDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string MethodDecl::toString() const {
    std::string result = "Method(" + name + "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) result += ", ";
        result += parameters[i];
    }
    result += "): " + returnType + ")";
    return result;
}

// --- ClassDecl ---
ClassDecl::ClassDecl(const std::string& n) : name(n) {}
void ClassDecl::addField(std::unique_ptr<FieldDecl> field) { fields.push_back(std::move(field)); }
void ClassDecl::addMethod(std::unique_ptr<MethodDecl> method) { methods.push_back(std::move(method)); }
void ClassDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string ClassDecl::toString() const {
    return "Class(" + name + ", fields=" + std::to_string(fields.size()) + 
           ", methods=" + std::to_string(methods.size()) + ")";
}

// --- MethodCall ---
MethodCall::MethodCall(const std::string& obj, const std::string& meth) : object(obj), method(meth) {}
void MethodCall::addArgument(std::unique_ptr<Expression> arg) { arguments.push_back(std::move(arg)); }
void MethodCall::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string MethodCall::toString() const {
    std::string result = object + "." + method + "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += arguments[i]->toString();
    }
    return result + ")";
}

// --- FieldAccess ---
FieldAccess::FieldAccess(const std::string& obj, const std::string& fld) : object(obj), field(fld) {}
void FieldAccess::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string FieldAccess::toString() const { return object + "." + field; }

// --- NewObject ---
NewObject::NewObject(const std::string& name) : className(name) {}
void NewObject::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string NewObject::toString() const { return "new " + className + "()"; }

// --- ReturnStmt ---
ReturnStmt::ReturnStmt(std::unique_ptr<Expression> val) : value(std::move(val)) {}
void ReturnStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string ReturnStmt::toString() const {
    return value ? "return " + value->toString() : "return";
}

// ========== MethodCallStmt ==========
MethodCallStmt::MethodCallStmt(std::unique_ptr<MethodCall> c) : call(std::move(c)) {}
void MethodCallStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string MethodCallStmt::toString() const { return call->toString(); }

// ========== FieldAssignStmt ==========
FieldAssignStmt::FieldAssignStmt(std::unique_ptr<FieldAccess> f, std::unique_ptr<Expression> v)
    : field(std::move(f)), value(std::move(v)) {}
void FieldAssignStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string FieldAssignStmt::toString() const {
    return field->toString() + " = " + value->toString();
}

// ========== NewStmt ==========
NewStmt::NewStmt(std::unique_ptr<NewObject> n) : newObj(std::move(n)) {}
void NewStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string NewStmt::toString() const { return newObj->toString(); }