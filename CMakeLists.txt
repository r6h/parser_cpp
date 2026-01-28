cmake_minimum_required(VERSION 3.10)
project(markdown_parser)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES
    src/tokenizer.cpp
    src/parser.cpp
    src/renderer.cpp
)

# Binary 1: md2html
add_executable(md2html tools/md2html.cpp ${SOURCES})

# Binary 2: test_runner
add_executable(test_runner tests/test_main.cpp ${SOURCES})

# Binary 3: debug_ast
add_executable(debug_ast src/debug_ast.cpp ${SOURCES})