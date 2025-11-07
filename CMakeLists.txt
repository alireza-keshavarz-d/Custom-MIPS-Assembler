cmake_minimum_required(VERSION 3.31)
project(MIPS_ASSEMBLER)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(assembler
        src/lexer.cpp
        src/parser.cpp
        src/symbol_table.cpp
        src/code_gen.cpp
        src/assembler.cpp
        src/main.cpp
)

target_sources(assembler PRIVATE
        src/common.h
        src/lexer.h
        src/parser.h
        src/symbol_table.h
        src/code_gen.h
        src/assembler.h
)

install(TARGETS assembler DESTINATION bin)