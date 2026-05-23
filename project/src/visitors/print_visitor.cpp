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
    out << "BinaryOp(" << binOpTypeToString(node.op) << ")\n";
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

void PrintVisitor::visit(WhileStmt& node) {
    out << "While (condition):\n";
    indent++;
    printIndent();
    node.condition->accept(*this);
    indent--;
    
    out << "Body:\n";
    indent++;
    for (auto& stmt : node.body) {
        printIndent();
        stmt->accept(*this);
    }
    indent--;
}

void PrintVisitor::visit(BlockStatement& node) {
    out << "Block:\n";
    indent++;
    for (auto& stmt : node.statements) {
        printIndent();
        stmt->accept(*this);
    }
    indent--;
}

void PrintVisitor::visit(FieldDecl& node) {
    out << "Field(" << node.name << ": " << node.type << ")\n";
}
void PrintVisitor::visit(MethodDecl& node) {
    out << "Method(" << node.name << ")\n";
}
void PrintVisitor::visit(ClassDecl& node) {
    out << "Class(" << node.name << ")\n";
    indent++;
    for (auto& field : node.fields) {
        printIndent();
        field->accept(*this);
    }
    for (auto& method : node.methods) {
        printIndent();
        method->accept(*this);
    }
    indent--;
}

void PrintVisitor::visit(MethodCall& node) {
    out << "MethodCall(" << node.object << "." << node.method << ")\n";
}
void PrintVisitor::visit(FieldAccess& node) {
    out << "FieldAccess(" << node.object << "." << node.field << ")\n";
}
void PrintVisitor::visit(NewObject& node) {
    out << "NewObject(" << node.className << ")\n";
}
void PrintVisitor::visit(ReturnStmt& node) {
    out << "Return\n";
}

void PrintVisitor::visit(MethodCallStmt& node) {
    out << "MethodCallStmt: ";
    node.call->accept(*this);
}

void PrintVisitor::visit(FieldAssignStmt& node) {
    out << "FieldAssignStmt: ";
    node.field->accept(*this);
    out << " = ";
    node.value->accept(*this);
}

void PrintVisitor::visit(NewStmt& node) {
    out << "NewStmt: ";
    node.newObj->accept(*this);
}