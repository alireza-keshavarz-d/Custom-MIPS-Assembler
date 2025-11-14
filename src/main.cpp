#include "assembler.h"

#include <iostream>
#include <fstream>
#include <string>


int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Usage:\n"
                  << "  " << argv[0]
                  << " path/to/input.asm"
                     " path/to/inst_template.vhd"
                     " path/to/inst_mem.vhd"
                     " path/to/data_template.vhd"
                     " path/to/data_mem.vhd\n";
        return 1;
    }


    std::string input_file  = argv[1];
    std::string inst_tmpl   = argv[2];
    std::string inst_out    = argv[3];
    std::string data_tmpl   = argv[4];
    std::string data_out    = argv[5];

    std::ifstream in(input_file);
    if (!in) {
        std::cerr << "Failed to open input file: " << input_file << "\n";
        return 1;
    }
    std::string input((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    try {
        Assembler assembler(input);
        assembler.assemble(
            inst_out,
            data_out,
            inst_tmpl,
            data_tmpl
        );

        std::cout << "Assembly Successful\n";

    } catch (const std::exception& e) {
        std::cerr << "Assembly error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}