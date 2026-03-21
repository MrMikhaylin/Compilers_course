#include <gtest/gtest.h>
#include "lexer.hpp"
#include "parser.hpp"
#include "visitors/print_visitor.hpp"
#include <sstream>

// Тест: объявление переменной
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

// Тест: присваивание
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

// Тест: if-else
TEST(ParserTest, IfStmt) {
    std::string code = "if (x == 0) { print(1); } else { print(2); }";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 1);
    auto* ifstmt = dynamic_cast<IfStmt*>(program->statements[0].get());
    ASSERT_NE(ifstmt, nullptr);
    
    // Проверяем, что then-ветка не пустая
    ASSERT_EQ(ifstmt->thenBranch.size(), 1);
    auto* printThen = dynamic_cast<PrintStmt*>(ifstmt->thenBranch[0].get());
    ASSERT_NE(printThen, nullptr);
    
    // Проверяем, что else-ветка не пустая
    ASSERT_EQ(ifstmt->elseBranch.size(), 1);
    auto* printElse = dynamic_cast<PrintStmt*>(ifstmt->elseBranch[0].get());
    ASSERT_NE(printElse, nullptr);
}

// Тест: арифметическое выражение
TEST(ParserTest, BinaryExpression) {
    std::string code = "x = 5 + 3 * 2;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    auto* assign = dynamic_cast<Assignment*>(program->statements[0].get());
    auto* binop = dynamic_cast<BinaryOp*>(assign->value.get());
    
    // Проверяем, что приоритет операций соблюден: 3 * 2 вычисляется раньше
    ASSERT_NE(binop, nullptr);
    EXPECT_EQ(binop->op, "+");
    
    auto* right = dynamic_cast<BinaryOp*>(binop->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->op, "*");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}