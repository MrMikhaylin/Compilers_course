#include "lexer.hpp"
#include "parser.hpp"
#include "visitors/print_visitor.hpp"
#include "visitors/interpreter.hpp"
#include "visitors/scope_builder.hpp"
#include <fstream>
#include <iostream>
#include <memory>

int main() {
    // Тестовая программа с циклом while и операторами сравнения
    std::string testProgram = R"(
        declare x: int;
        declare y: int;
        x = 0;
        y = 0;
        while (x < 5) {
            y = y + x;
            x = x + 1;
        }
        print(y);
    )";
    
    try {
        std::cout << "=== Лексический анализ ===" << std::endl;
        Lexer lexer(testProgram);
        std::vector<Token> tokens = lexer.tokenize();
        
        std::cout << "Токены:" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "  " << token.toString() << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "=== Синтаксический анализ ===" << std::endl;
        Parser parser(tokens);
        auto program = parser.parse();
        std::cout << "Парсинг успешен!" << std::endl << std::endl;
        
        std::cout << "=== AST дерево ===" << std::endl;
        PrintVisitor consolePrinter(std::cout);
        program->accept(consolePrinter);
        std::cout << std::endl;
        
        std::cout << "=== Построение дерева скоупов ===" << std::endl;
        ScopeBuilder scopeBuilder;
        program->accept(scopeBuilder);
        
        std::cout << "Дерево скоупов:" << std::endl;
        scopeBuilder.getGlobalScope()->dump();
        std::cout << std::endl;
        
        scopeBuilder.reportErrors();
        std::cout << std::endl;
        
        std::cout << "=== Выполнение программы ===" << std::endl;
        Interpreter interpreter;
        program->accept(interpreter);
        std::cout << std::endl;
        
        std::cout << "=== Программа успешно завершена ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n!!! ОШИБКА: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}