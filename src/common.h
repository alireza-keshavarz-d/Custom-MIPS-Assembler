#ifndef COMMON_H
#define COMMON_H


#include <string>
#include <optional>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>


enum class TokenType {
    EoF,
    IDENT,
    NUMBER,
    REGISTER,
    INST,
    DOT,
    COMMA,
    COLON,
    LPAREN,
    RPAREN,
    ILLEGAL
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
};

enum class NodeType {
    INSTRUCTION,
    DIRECTIVE,
    LABEL
};

enum class Section {
    TEXT,
    DATA,
    NONE // initial or undefined
};

struct Node {
    NodeType type;
    std::optional<std::string> label_name;
    uint32_t address = 0;
    Section section = Section::TEXT;
    virtual ~Node() = default;
};

inline const std::unordered_set<std::string_view> RTYPE_INSTRUCTIONS {
    "mult","add","sub","sll","srl","and","or","not"
};

inline const std::unordered_set<std::string_view> ITYPE_INSTRUCTIONS { "lw","sw","beq" };

inline const std::unordered_set<std::string_view> INSTRUCTIONS = [] {
    std::unordered_set<std::string_view> s;
    s.insert(RTYPE_INSTRUCTIONS.begin(), RTYPE_INSTRUCTIONS.end());
    s.insert(ITYPE_INSTRUCTIONS.begin(), ITYPE_INSTRUCTIONS.end());
    return s;
}();

inline const std::unordered_set<std::string_view> REGISTERS {
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7"
};

const std::unordered_map<std::string_view, uint32_t> FUNCT_CODES = {
    {"mult", 0x18},
    {"add", 0x20},
    {"sub", 0x22},
    {"sll", 0x04},
    {"srl", 0x06},
    {"and", 0x24},
    {"or", 0x25},
    {"not", 0x27},
};

const std::unordered_map<std::string_view, uint32_t> OPCODES = {
    {"lw", 0x23},
    {"sw", 0x2B},
    {"beq", 0x04}
};

const std::unordered_map<std::string_view, uint32_t> REG_MAP = {
    {"a0", 0}, {"a1", 1}, {"a2", 2}, {"a3", 3}, {"a4", 4}, {"a5", 5}, {"a6", 6}, {"a7", 7},
    {"r0", 8}, {"r1", 9}, {"r2", 10}, {"r3", 11}, {"r4", 12}, {"r5", 13}, {"r6", 14}, {"r7", 15},
    {"s0", 16}, {"s1", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
    {"t0", 24}, {"t1", 25}, {"t2", 26}, {"t3", 27}, {"t4", 28}, {"t5", 29}, {"t6", 30}, {"t7", 31}
};

// the assembler will output to the files, in lines that starts (whitespace is allowed) with "###"
constexpr std::string_view subs_token = "###";

#endif // COMMON_H
