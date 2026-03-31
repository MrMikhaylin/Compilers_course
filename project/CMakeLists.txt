cmake_minimum_required(VERSION 3.10)
project(MiniLangCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(include/visitors)

set(SOURCES
    src/lexer.cpp
    src/ast.cpp
    src/parser.cpp
    src/scope.cpp
    src/visitors/print_visitor.cpp
    src/visitors/interpreter.cpp
    src/visitors/scope_builder.cpp
)

add_executable(minilang 
    src/main.cpp
    ${SOURCES}
)

target_compile_options(minilang PRIVATE -Wall -Wextra)

enable_testing()

find_package(GTest QUIET)
add_executable(tests tests/test_parser.cpp ${SOURCES})
target_link_libraries(tests GTest::GTest GTest::Main)
add_test(NAME parser_tests COMMAND tests)
target_compile_options(tests PRIVATE -Wall -Wextra)
message(STATUS "Тесты включены")