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

// Тест: while цикл
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

// Тест: арифметическое выражение с приоритетом операций
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
    
    // Проверяем, что приоритет операций соблюден: 3 * 2 вычисляется раньше
    // Верхний уровень - это сложение
    EXPECT_EQ(binop->op, BinOpType::PLUS);
    
    // Правый операнд - умножение
    auto* right = dynamic_cast<BinaryOp*>(binop->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->op, BinOpType::MULTIPLY);
}

// Тест: операторы сравнения
TEST(ParserTest, ComparisonOperators) {
    std::string code = "x = 5 < 10; y = 5 > 10; z = 5 <= 5;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 3);
    
    // Проверяем оператор <
    auto* assign1 = dynamic_cast<Assignment*>(program->statements[0].get());
    ASSERT_NE(assign1, nullptr);
    auto* binop1 = dynamic_cast<BinaryOp*>(assign1->value.get());
    ASSERT_NE(binop1, nullptr);
    EXPECT_EQ(binop1->op, BinOpType::LESS);
    
    // Проверяем оператор >
    auto* assign2 = dynamic_cast<Assignment*>(program->statements[1].get());
    ASSERT_NE(assign2, nullptr);
    auto* binop2 = dynamic_cast<BinaryOp*>(assign2->value.get());
    ASSERT_NE(binop2, nullptr);
    EXPECT_EQ(binop2->op, BinOpType::GREATER);
    
    // Проверяем оператор <=
    auto* assign3 = dynamic_cast<Assignment*>(program->statements[2].get());
    ASSERT_NE(assign3, nullptr);
    auto* binop3 = dynamic_cast<BinaryOp*>(assign3->value.get());
    ASSERT_NE(binop3, nullptr);
    EXPECT_EQ(binop3->op, BinOpType::LESS_EQUAL);
}

// Тест: блок операторов
TEST(ParserTest, BlockStatement) {
    std::string code = "{ declare x: int; x = 5; print(x); }";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    ASSERT_EQ(program->statements.size(), 3);
    auto* decl = dynamic_cast<VarDecl*>(program->statements[0].get());
    ASSERT_NE(decl, nullptr);
    auto* assign = dynamic_cast<Assignment*>(program->statements[1].get());
    ASSERT_NE(assign, nullptr);
    auto* print = dynamic_cast<PrintStmt*>(program->statements[2].get());
    ASSERT_NE(print, nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}