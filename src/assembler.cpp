#include "assembler.h"
#include "code_gen.h"

#include <bitset>
#include <fstream>
#include <iomanip>
#include <utility>


Assembler::Assembler(std::string  input) : input_(std::move(input)) {}

void Assembler::assemble(const std::string& instruction_file, const std::string& data_file) const {
    Lexer lexer(input_);
    Parser parser(lexer);
    AST ast = parser.parse();

    CodeGenerator code_gen;
    SymbolTable sym_table = code_gen.pass1(ast);
    auto [instructions, data] = code_gen.pass2(ast, sym_table);

    // write inst to file
    std::ofstream inst_out(instruction_file, std::ios::out);
    if (!inst_out) throw std::runtime_error("Failed to open instruction output file: " + instruction_file);
    for (size_t i = 0; i < instructions.size(); i += 4) {
        // 32-bit word from be bytes
        uint32_t word = (static_cast<uint32_t>(instructions[i]) << 24) |
                        (static_cast<uint32_t>(instructions[i + 1]) << 16) |
                        (static_cast<uint32_t>(instructions[i + 2]) << 8) |
                        static_cast<uint32_t>(instructions[i + 3]);
        inst_out << std::bitset<32>(word) << std::endl;
    }

    // write data file
    std::ofstream data_out(data_file, std::ios::out);
    if (!data_out) throw std::runtime_error("Failed to open data output file: " + data_file);
    for (size_t i = 0; i < data.size(); i += 4) {
        // 32-bit word from be bytes
        uint32_t word = (static_cast<uint32_t>(data[i]) << 24) |
                        (static_cast<uint32_t>(data[i + 1]) << 16) |
                        (static_cast<uint32_t>(data[i + 2]) << 8) |
                        static_cast<uint32_t>(data[i + 3]);
        data_out << std::bitset<32>(word) << std::endl;
    }
}
