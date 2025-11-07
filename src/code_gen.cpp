#include "code_gen.h"

#include <sstream>
#include <bitset>


uint32_t CodeGenerator::get_reg_num(const std::string& reg) const {
    const auto it = REG_MAP.find(reg);
    if (it == REG_MAP.end()) throw std::runtime_error("Invalid register: " + reg);
    return it->second;
}

SymbolTable CodeGenerator::pass1(const AST& ast) {
    SymbolTable sym_table;
    uint32_t text_addr = 0;
    uint32_t data_addr = 0;
    auto current_section = Section::TEXT;
    
    for (auto& node : ast.nodes) {
        node->section = current_section;
        
        // assign address based on section
        if (current_section == Section::DATA) {
            node->address = data_addr;
        } else {
            node->address = text_addr;
        }

        if (node->type == NodeType::LABEL) {
            const auto* label = dynamic_cast<LabelNode*>(node.get());
            sym_table.add(label->name, node->address);
        } else if (node->type == NodeType::INSTRUCTION) {
            if (current_section == Section::DATA) {
                throw std::runtime_error("Instructions not allowed in .data section");
            }
            text_addr += 4;
        } else if (node->type == NodeType::DIRECTIVE) {
            const auto* dir = dynamic_cast<DirectiveNode*>(node.get());
            if (dir->name == "text") {
                current_section = Section::TEXT;
            } else if (dir->name == "data") {
                current_section = Section::DATA;
            } else if (dir->name == "word") {
                if (current_section == Section::TEXT) {
                    throw std::runtime_error(".word directive not allowed in .text section");
                }
                data_addr += 4 * dir->values.size();
            }
        }
    }
    return sym_table;
}

BinaryOutput CodeGenerator::pass2(const AST& ast, const SymbolTable& sym_table) {
    // calculate size for each section
    uint32_t text_size = 0;
    uint32_t data_size = 0;
    
    for (const auto& node : ast.nodes) {
        if (node->type == NodeType::INSTRUCTION) {
            if (node->section == Section::TEXT) {
                text_size += 4;
            }
        } else if (node->type == NodeType::DIRECTIVE) {
            const auto* dir = dynamic_cast<DirectiveNode*>(node.get());
            if (dir->name == "word" && node->section == Section::DATA) {
                data_size += 4 * dir->values.size();
            }
        }
    }

    BinaryOutput output;
    output.instructions.resize(text_size);
    output.data.resize(data_size);
    
    uint32_t text_pos = 0;
    uint32_t data_pos = 0;

    for (const auto& node : ast.nodes) {
        if (node->type == NodeType::INSTRUCTION) {
            if (node->section == Section::TEXT) {
                uint32_t machine_code;
                if (auto* r_inst = dynamic_cast<RTypeInst*>(node.get())) {
                    machine_code = encode_r(r_inst);
                } else if (auto* i_inst = dynamic_cast<ITypeInst*>(node.get())) {
                    machine_code = encode_i(i_inst, node->address, sym_table);
                } else {
                    throw std::runtime_error("Unknown instruction type");
                }
                write_uint32(output.instructions, text_pos, machine_code);
                text_pos += 4;
            }
        } else if (node->type == NodeType::DIRECTIVE) {
            auto* dir = static_cast<DirectiveNode*>(node.get());
            if (dir->name == "word" && node->section == Section::DATA) {
                for (const auto& val : dir->values) {
                    uint32_t word_val = encode_word(val, sym_table);
                    write_uint32(output.data, data_pos, word_val);
                    data_pos += 4;
                }
            }
        }
    }

    return output;
}

uint32_t CodeGenerator::encode_r(const RTypeInst* inst) const {
    auto funct_it = FUNCT_CODES.find(inst->mnemonic);
    if (funct_it == FUNCT_CODES.end()) throw std::runtime_error("Unknown R-type: " + inst->mnemonic);

    const uint32_t opcode = 0;
    uint32_t rs_num = get_reg_num(inst->rs);
    uint32_t rt_num;
    const uint32_t rd_num = get_reg_num(inst->rd);
    uint32_t shamt = 0;
    const uint32_t funct = funct_it->second;

    // handling sll/srl
    if (inst->mnemonic == "sll" || inst->mnemonic == "srl") {
        try {
            // parse as number
            const int32_t shift_amt = std::stoll(inst->rt, nullptr, 0);
            if (shift_amt < 0 || shift_amt > 31) {
                throw std::runtime_error("Shift amount must be between 0 and 31");
            }
            shamt = static_cast<uint32_t>(shift_amt);
            rt_num = get_reg_num(inst->rs);
            rs_num = 0;
        } catch (const std::invalid_argument&) {
            // parse as register
            rt_num = get_reg_num(inst->rt);
            // sll rd, rs(value), rt(amount) --> rs=rt(amount), rt=rs(value)
            std::swap(rs_num, rt_num);
        }
    } else {
        rt_num = get_reg_num(inst->rt);
    }

    // for 'not', use nor rd, rs, rt -> ~(rs | rt)
    // if user wants pure not, set rt=r0

    return (opcode << 26) | (rs_num << 21) | (rt_num << 16) | (rd_num << 11) | (shamt << 6) | funct;
}

uint32_t CodeGenerator::encode_i(const ITypeInst* inst, uint32_t current_addr, const SymbolTable& sym_table) const {
    const auto opcode_it = OPCODES.find(inst->mnemonic);
    if (opcode_it == OPCODES.end()) throw std::runtime_error("Unknown I-type: " + inst->mnemonic);

    const uint32_t opcode = opcode_it->second;
    const uint32_t rs_num = inst->rs.empty() ? 0 : get_reg_num(inst->rs);
    const uint32_t rt_num = get_reg_num(inst->rt);
    int32_t imm;

    if (inst->is_label_ref) {
        const auto addr_opt = sym_table.get(inst->imm_or_label);
        if (!addr_opt) throw std::runtime_error("Unresolved label: " + inst->imm_or_label);
        const uint32_t label_addr = *addr_opt;

        if (inst->mnemonic == "beq") {
            imm = (static_cast<int32_t>(label_addr) - static_cast<int32_t>(current_addr + 4)) / 4;
        } else {
            imm = label_addr; // absolute for lw/sw
        }
    } else {
        try {
            imm = std::stoll(inst->imm_or_label, nullptr, 0); // detect base
        } catch (...) {
            throw std::runtime_error("Invalid immediate: " + inst->imm_or_label);
        }
    }

    if (imm < -32768 || imm > 32767) throw std::runtime_error("Immediate overflow: " + std::to_string(imm));

    // beq: rs, rt, imm;
    // lw/sw: rs(base), rt, imm
    return (opcode << 26) | (rs_num << 21) | (rt_num << 16) | (static_cast<uint32_t>(imm) & 0xFFFF);
}

uint32_t CodeGenerator::encode_word(const std::string& val, const SymbolTable& sym_table) const {
    try {
        return std::stoll(val, nullptr, 0);
    } catch (...) {
        const auto addr_opt = sym_table.get(val);
        if (!addr_opt) throw std::runtime_error("Unresolved label in .word: " + val);
        return *addr_opt;
    }
}

void CodeGenerator::write_uint32(std::vector<uint8_t>& buffer, const size_t pos, const uint32_t value) const {
    // big-endian
    buffer[pos + 0] = (value >> 24) & 0xFF;
    buffer[pos + 1] = (value >> 16) & 0xFF;
    buffer[pos + 2] = (value >> 8) & 0xFF;
    buffer[pos + 3] = value & 0xFF;
}