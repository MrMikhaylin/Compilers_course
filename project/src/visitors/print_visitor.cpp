#include "visitors/print_visitor.hpp"
#include <iostream>

PrintVisitor::PrintVisitor(std::ostream& os) 
    : out(os), indent(0) {}

void PrintVisitor::printIndent() {
    for (int i = 0; i < indent; i++) {
        out << "  ";
    }
}

void PrintVisitor::visit(Program& node) {
    out << "Program:\n";
    indent++;
    for (auto& stmt : node.statements) {
        printIndent();
        stmt->accept(*this);
    }
    indent--;
}

void PrintVisitor::visit(NumberLiteral& node) {
    out << "Number(" << node.value << ")\n";
}

void PrintVisitor::visit(Variable& node) {
    out << "Variable(" << node.name << ")\n";
}

void PrintVisitor::visit(BinaryOp& node) {
    out << "BinaryOp(" << node.op << ")\n";
    indent++;
    printIndent();
    node.left->accept(*this);
    printIndent();
    node.right->accept(*this);
    indent--;
}

void PrintVisitor::visit(VarDecl& node) {
    out << "VarDecl(" << node.name << ": " << node.type << ")\n";
}

void PrintVisitor::visit(Assignment& node) {
    out << "Assignment(" << node.name << " = \n";
    indent++;
    printIndent();
    node.value->accept(*this);
    indent--;
    out << ")\n";
}

void PrintVisitor::visit(PrintStmt& node) {
    out << "Print:\n";
    indent++;
    printIndent();
    node.expr->accept(*this);
    indent--;
}

void PrintVisitor::visit(IfStmt& node) {
    out << "If (condition):\n";
    indent++;
    printIndent();
    node.condition->accept(*this);
    indent--;
    
    out << "Then:\n";
    indent++;
    for (auto& stmt : node.thenBranch) {
        printIndent();
        stmt->accept(*this);
    }
    indent--;
    
    if (!node.elseBranch.empty()) {
        out << "Else:\n";
        indent++;
        for (auto& stmt : node.elseBranch) {
            printIndent();
            stmt->accept(*this);
        }
        indent--;
    }
}