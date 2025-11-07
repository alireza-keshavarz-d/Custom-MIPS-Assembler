#include "assembler.h"

#include <iostream>
#include <fstream>
#include <string>


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input.asm output_base" << std::endl;
        std::cerr << "Output files: <output_base>_inst.bin (instructions) and <output_base>_data.bin (data)" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_base = argv[2];
    std::string instruction_file = output_base + "_inst.bin";
    std::string data_file = output_base + "_data.bin";

    std::ifstream in(input_file);
    if (!in) {
        std::cerr << "Failed to open input file: " << input_file << std::endl;
        return 1;
    }

    std::string input((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    try {
        Assembler assembler(input);
        assembler.assemble(instruction_file, data_file);
        std::cout << "Assembly successful." << std::endl;
        std::cout << "Instruction memory: " << instruction_file << std::endl;
        std::cout << "Data memory: " << data_file << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Assembly error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}