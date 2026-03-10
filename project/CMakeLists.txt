cmake_minimum_required(VERSION 3.10)
project(MiniLangCompiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(minilang 
    src/main.cpp
    src/lexer.cpp
    src/ast.cpp
    src/parser.cpp
)