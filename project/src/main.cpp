#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include <iostream>
#include <memory>

int main() {
    // Тестовая программа
    std::string testProgram = R"(
        declare x: int;
        x = 42;
        if (x == 0) {
            print(x);
        } else {
            print(x + 1);
        }
    )";
    
    try {
        // Лексический анализ
        Lexer lexer(testProgram);
        std::vector<Token> tokens = lexer.tokenize();
        
        std::cout << "Токены:" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "  " << token.toString() << std::endl;
        }
        std::cout << "\n";
        
        // Синтаксический анализ
        Parser parser(tokens);
        auto program = parser.parse();
        
        std::cout << "AST дерево:" << std::endl;
        PrintVisitor printer;
        program->accept(printer);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}