#include <gtest/gtest.h>
#include "lexer.hpp"
#include "parser.hpp"
#include "visitors/print_visitor.hpp"

TEST(ParserTest, VarDecl) {
    std::string code = "declare x: int;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 1);
    auto* decl = dynamic_cast<VarDecl*>(program->statements[0].get());
    ASSERT_NE(decl, nullptr);
    EXPECT_EQ(decl->name, "x");
    EXPECT_EQ(decl->type, "int");
}

TEST(ParserTest, Assignment) {
    std::string code = "x = 42;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 1);
    auto* assign = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(assign, nullptr);
    EXPECT_EQ(assign->name, "x");
    
    auto* num = dynamic_cast<NumberLiteral*>(assign->value.get());
    ASSERT_NE(num, nullptr);
    EXPECT_EQ(num->value, 42);
}

TEST(ParserTest, IfStmt) {
    std::string code = "if (x == 0) { print(1); } else { print(2); }";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 1);
    auto* ifstmt = dynamic_cast<IfStmt*>(program->statements[0].get());
    ASSERT_NE(ifstmt, nullptr);
    
    ASSERT_EQ(ifstmt->thenBranch.size(), 1);
    auto* printThen = dynamic_cast<PrintStmt*>(ifstmt->thenBranch[0].get());
    ASSERT_NE(printThen, nullptr);
    
    ASSERT_EQ(ifstmt->elseBranch.size(), 1);
    auto* printElse = dynamic_cast<PrintStmt*>(ifstmt->elseBranch[0].get());
    ASSERT_NE(printElse, nullptr);
}

TEST(ParserTest, WhileStmt) {
    std::string code = "while (x < 10) { x = x + 1; }";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 1);
    auto* whileStmt = dynamic_cast<WhileStmt*>(program->statements[0].get());
    ASSERT_NE(whileStmt, nullptr);
    ASSERT_EQ(whileStmt->body.size(), 1);
}

TEST(ParserTest, BinaryExpression) {
    std::string code = "x = 5 + 3 * 2;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    auto* assign = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(assign, nullptr);
    auto* binop = dynamic_cast<BinaryOp*>(assign->value.get());
    ASSERT_NE(binop, nullptr);
    
    EXPECT_EQ(binop->op, BinOpType::PLUS);
    
    auto* right = dynamic_cast<BinaryOp*>(binop->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->op, BinOpType::MULTIPLY);
}

TEST(ParserTest, BlockStatement) {
    std::string code = "{ declare x: int; x = 5; print(x); }";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 3);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}