#ifndef CODE_GEN_H
#define CODE_GEN_H


#include "parser.h"
#include "symbol_table.h"

#include <vector>


struct BinaryOutput {
    std::vector<uint8_t> instructions;
    std::vector<uint8_t> data;
};

class CodeGenerator {
public:
    SymbolTable pass1(const AST& ast);
    BinaryOutput pass2(const AST& ast, const SymbolTable& sym_table);

private:
    uint32_t get_reg_num(const std::string& reg) const;
    uint32_t encode_r(const RTypeInst* inst) const;
    uint32_t encode_i(const ITypeInst* inst, uint32_t current_addr, const SymbolTable& sym_table) const;
    uint32_t encode_word(const std::string& val, const SymbolTable& sym_table) const;
    // in big-endian
    void write_uint32(std::vector<uint8_t>& buffer, size_t pos, uint32_t value) const;
};

#endif // CODE_GEN_H