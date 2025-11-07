cmake_minimum_required(VERSION 3.31)
project(MIPS_ASSEMBLER)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(assembler
        lexer.cpp
        parser.cpp
        symbol_table.cpp
        code_gen.cpp
        assembler.cpp
        main.cpp
)

target_sources(assembler PRIVATE
        common.h
        lexer.h
        parser.h
        symbol_table.h
        code_gen.h
        assembler.h
)

install(TARGETS assembler DESTINATION bin)