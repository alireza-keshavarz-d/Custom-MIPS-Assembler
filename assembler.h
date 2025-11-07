#ifndef ASSEMBLER_H
#define ASSEMBLER_H


#include "parser.h"


class Assembler {
public:
    explicit Assembler(std::string  input);

    void assemble(const std::string& instruction_file, const std::string& data_file) const;

private:
    std::string input_;
};

#endif // ASSEMBLER_H