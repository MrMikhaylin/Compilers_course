#include "lexer.hpp"
#include "parser.hpp"
#include "visitors/print_visitor.hpp"
#include "visitors/interpreter.hpp"
#include "visitors/scope_builder.hpp"
#include <fstream>
#include <iostream>
#include <memory>

int main() {
    // Тестовая программа для итерации 3
    // Проверяет: shadowing, вложенные скоупы, циклы, условия, восстановление переменных
    std::string testProgram = R"(
        declare a: int;
        declare b: int;
        a = 10;
        b = 20;
        print(a + b);
        
        declare x: int;
        x = 100;
        print(x);
        
        {
            declare x: int;
            x = 200;
            print(x);
        }
        print(x);
        
        {
            declare y: int;
            y = 5;
            print(y);
        }
        
        {
            declare z: int;
            z = 777;
            print(z);
        }
        
        declare level1: int;
        level1 = 1;
        print(level1);
        
        {
            declare level2: int;
            level2 = 2;
            print(level1);
            print(level2);
            
            {
                declare level3: int;
                level3 = 3;
                print(level1);
                print(level2);
                print(level3);
                
                declare level1: int;
                level1 = 100;
                print(level1);
            }
            
            print(level1);
            print(level2);
        }
        print(level1);
        
        declare counter: int;
        declare sum: int;
        counter = 0;
        sum = 0;
        
        while (counter < 5) {
            declare temp: int;
            temp = counter * 2;
            sum = sum + temp;
            counter = counter + 1;
        }
        print(sum);
        
        declare value: int;
        value = 10;
        
        if (value > 5) {
            declare inner: int;
            inner = 100;
            print(inner);
        } else {
            declare inner: int;
            inner = 200;
            print(inner);
        }
        
        declare global: int;
        global = 1000;
        
        {
            declare local1: int;
            local1 = 100;
            
            {
                declare local2: int;
                local2 = 10;
                global = global + local1 + local2;
            }
        }
        print(global);
    )";
    
    try {
        std::cout << "========================================" << std::endl;
        std::cout << "1. ЛЕКСИЧЕСКИЙ АНАЛИЗ (Lexer)" << std::endl;
        std::cout << "========================================" << std::endl;
        Lexer lexer(testProgram);
        std::vector<Token> tokens = lexer.tokenize();
        
        std::cout << "Всего токенов: " << tokens.size() << std::endl;
        for (size_t i = 0; i < std::min(tokens.size(), size_t(30)); ++i) {
            std::cout << "  " << tokens[i].toString() << std::endl;
        }
        if (tokens.size() > 30) {
            std::cout << "  ... и еще " << tokens.size() - 30 << " токенов" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "========================================" << std::endl;
        std::cout << "2. СИНТАКСИЧЕСКИЙ АНАЛИЗ (Parser)" << std::endl;
        std::cout << "========================================" << std::endl;
        Parser parser(tokens);
        auto program = parser.parse();
        std::cout << "✓ Парсинг успешно завершен" << std::endl << std::endl;
        
        std::cout << "========================================" << std::endl;
        std::cout << "3. ВЫВОД AST ДЕРЕВА (PrintVisitor)" << std::endl;
        std::cout << "========================================" << std::endl;
        std::ofstream astFile("ast_full.txt");
        if (astFile.is_open()) {
            PrintVisitor filePrinter(astFile);
            program->accept(filePrinter);
            astFile.close();
            std::cout << "✓ Полное AST сохранено в файл: ast_full.txt" << std::endl;
        }
        
        PrintVisitor consolePrinter(std::cout);
        program->accept(consolePrinter);
        std::cout << std::endl;
        
        std::cout << "========================================" << std::endl;
        std::cout << "4. СЕМАНТИЧЕСКИЙ АНАЛИЗ (ScopeBuilder)" << std::endl;
        std::cout << "========================================" << std::endl;
        ScopeBuilder scopeBuilder;
        program->accept(scopeBuilder);
        
        std::cout << "ДЕРЕВО СКОУПОВ:" << std::endl;
        scopeBuilder.getGlobalScope()->dump();
        std::cout << std::endl;
        
        scopeBuilder.reportErrors();
        std::cout << std::endl;
        
        std::cout << "========================================" << std::endl;
        std::cout << "5. ВЫПОЛНЕНИЕ ПРОГРАММЫ (Interpreter)" << std::endl;
        std::cout << "========================================" << std::endl;
        Interpreter interpreter;
        program->accept(interpreter);
        std::cout << std::endl;
        
        std::cout << "========================================" << std::endl;
        std::cout << "ПРОГРАММА УСПЕШНО ЗАВЕРШЕНА" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n!!! ОШИБКА: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}