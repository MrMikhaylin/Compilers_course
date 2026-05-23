#include "lexer.hpp"
#include "parser.hpp"
#include "visitors/print_visitor.hpp"
#include "visitors/interpreter.hpp"
#include "visitors/scope_builder.hpp"
#include <fstream>
#include <iostream>
#include <memory>

int main() {
    // Тестовая программа из задания итерации 3
    // Проверяет: shadowing, двойное объявление, необъявленные переменные
    std::string testProgram = R"(
        // ===== 1. Базовое объявление и использование =====
        declare a: int;
        declare b: int;
        a = 10;
        b = 20;
        print(a + b);
        
        // ===== 2. Shadowing (переменная закрывает другую) =====
        declare x: int;
        x = 100;
        print(x);           // 100
        
        {
            declare x: int;   // shadowing!
            x = 200;
            print(x);         // 200
        }
        print(x);             // 100 (восстановился)
        
        // ===== 3. Двойное объявление в одном скоупе (должна быть ошибка) =====
        {
            declare y: int;
            // declare y: int;   // РАСКОММЕНТИРУЙ ДЛЯ ПРОВЕРКИ ОШИБКИ
            y = 5;
            print(y);
        }
        
        // ===== 4. Использование необъявленной переменной (должна быть ошибка) =====
        {
            // z = 777;          // РАСКОММЕНТИРУЙ ДЛЯ ПРОВЕРКИ ОШИБКИ
            declare z: int;
            z = 777;
            print(z);
        }
        
        // ===== 5. Вложенные скоупы и несколько уровней shadowing =====
        declare level1: int;
        level1 = 1;
        print(level1);        // 1
        
        {
            declare level2: int;
            level2 = 2;
            print(level1);    // 1 (видит внешний)
            print(level2);    // 2
            
            {
                declare level3: int;
                level3 = 3;
                print(level1);  // 1
                print(level2);  // 2
                print(level3);  // 3
                
                // shadowing на 3 уровне
                declare level1: int;
                level1 = 100;
                print(level1);  // 100 (локальный, затеняет внешний)
            }
            
            print(level1);      // 1 (внешний восстановился)
            print(level2);      // 2
        }
        print(level1);          // 1
        
        // ===== 6. Цикл while со своим скоупом =====
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
        print(sum);             // 0 + 2 + 4 + 6 + 8 = 20
        
        // ===== 7. If-else с разными скоупами =====
        declare value: int;
        value = 10;
        
        if (value > 5) {
            declare inner: int;
            inner = 100;
            print(inner);       // 100
        } else {
            declare inner: int;
            inner = 200;
            print(inner);
        }
        
        // ===== 8. Смешивание переменных из разных уровней =====
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
        print(global);          // 1000 + 100 + 10 = 1110
    )";
    
    try {
        std::cout << "=== Лексический анализ ===" << std::endl;
        Lexer lexer(testProgram);
        std::vector<Token> tokens = lexer.tokenize();
        
        std::cout << "Токены (всего " << tokens.size() << "):" << std::endl;
        // Выводим только первые 20 токенов для краткости
        for (size_t i = 0; i < std::min(tokens.size(), size_t(30)); ++i) {
            std::cout << "  " << tokens[i].toString() << std::endl;
        }
        if (tokens.size() > 30) {
            std::cout << "  ... и еще " << tokens.size() - 30 << " токенов" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "=== Синтаксический анализ ===" << std::endl;
        Parser parser(tokens);
        auto program = parser.parse();
        std::cout << "Парсинг успешен!" << std::endl << std::endl;
        
        std::cout << "=== AST дерево (первые 20 узлов) ===" << std::endl;
        // Сохраняем AST в файл для полного просмотра
        std::ofstream astFile("ast_full.txt");
        if (astFile.is_open()) {
            PrintVisitor filePrinter(astFile);
            program->accept(filePrinter);
            astFile.close();
            std::cout << "Полное AST сохранено в файл: ast_full.txt" << std::endl;
        }
        
        // Краткий вывод в консоль
        PrintVisitor consolePrinter(std::cout);
        program->accept(consolePrinter);
        std::cout << std::endl;
        
        std::cout << "=== Построение дерева скоупов (ScopeBuilder) ===" << std::endl;
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