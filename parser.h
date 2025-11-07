#ifndef PARSER_H
#define PARSER_H


#include "lexer.h"
#include "common.h"

#include <memory>
#include <vector>


struct RTypeInst : Node {
    std::string mnemonic;
    std::string rd;  // dst
    std::string rs;  // src 1
    std::string rt;  // src 2
    
    RTypeInst() { type = NodeType::INSTRUCTION; }
};

struct ITypeInst : Node {
    std::string mnemonic;
    std::string rt;         // Target/operand 1
    std::string rs;         // Base/operand 2 (optional for lw/sw if no (rs))
    std::string imm_or_label; // Immediate or label
    bool is_label_ref = false;
    
    ITypeInst() { type = NodeType::INSTRUCTION; }
};

struct DirectiveNode : Node {
    std::string name;
    std::vector<std::string> values;  // for .word
    
    DirectiveNode() { type = NodeType::DIRECTIVE; }
};

struct LabelNode : Node {
    std::string name;
    
    LabelNode() { type = NodeType::LABEL; }
};

struct AST {
    std::vector<std::unique_ptr<Node>> nodes;
};

class Parser {
public:
    explicit Parser(Lexer& lexer);

    AST parse();

    static bool is_valid_register(const std::string& reg);

private:
    void advance();
    void expect_token(TokenType expected, const std::string& error_msg) const;
    std::string expect_register(const std::string& error_msg);
    std::string expect_number_or_label(const std::string& error_msg);

    std::unique_ptr<Node> parse_statement();
    std::unique_ptr<RTypeInst> parse_rtype();
    std::unique_ptr<ITypeInst> parse_itype();
    std::unique_ptr<DirectiveNode> parse_directive();
    std::unique_ptr<LabelNode> parse_label();

    [[nodiscard]] bool is_rtype_instruction(const std::string& mnemonic) const;
    [[nodiscard]] bool is_itype_instruction(const std::string& mnemonic) const;

    Lexer& lexer_;
    Token current_token_;
    Token next_token_;
};

#endif // PARSER_H