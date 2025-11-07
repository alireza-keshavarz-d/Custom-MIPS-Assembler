#include "parser.h"

#include <sstream>
#include <stdexcept>
#include <algorithm>


Parser::Parser(Lexer& lexer)
    : lexer_(lexer), current_token_{}, next_token_{} {
    current_token_ = lexer_.next_token();
    next_token_ = lexer_.next_token();
}

void Parser::advance() {
    current_token_ = next_token_;
    next_token_ = lexer_.next_token();
}

void Parser::expect_token(TokenType expected, const std::string& error_msg) const {
    if (current_token_.type != expected) {
        std::ostringstream oss;
        oss << error_msg << " at line " << current_token_.line 
            << ". Expected " << static_cast<int>(expected) 
            << ", got " << static_cast<int>(current_token_.type)
            << " ('" << current_token_.literal << "')";
        throw std::runtime_error(oss.str());
    }
}

std::string Parser::expect_register(const std::string& error_msg) {
    expect_token(TokenType::REGISTER, error_msg);
    auto reg = current_token_.literal;
    
    // $ prefix for registers is removed
    if (!reg.empty() && reg[0] == '$') {
        reg = reg.substr(1);
    }
    
    if (!is_valid_register(reg)) {
        std::ostringstream oss;
        oss << "Invalid register name: " << reg << ". Valid registers: t0-t7, s0-s7, a0-a7, r0-r7";
        throw std::runtime_error(oss.str());
    }
    
    advance();
    return reg;
}

std::string Parser::expect_number_or_label(const std::string& error_msg) {
    if (current_token_.type == TokenType::NUMBER) {
        auto num = current_token_.literal;
        advance();
        return num;
    } else if (current_token_.type == TokenType::IDENT) {
        auto label = current_token_.literal;
        advance();
        return label;
    } else {
        throw std::runtime_error(error_msg);
    }
}

bool Parser::is_rtype_instruction(const std::string& mnemonic) const {
    return RTYPE_INSTRUCTIONS.contains(mnemonic);
}

bool Parser::is_itype_instruction(const std::string& mnemonic) const {
    return ITYPE_INSTRUCTIONS.contains(mnemonic);
}

bool Parser::is_valid_register(const std::string& reg) {
    // $ prefix for registers is removed
    auto reg_name = reg;
    if (!reg_name.empty() && reg_name[0] == '$') {
        reg_name = reg_name.substr(1);
    }
    return REGISTERS.contains(reg_name);
}

std::unique_ptr<RTypeInst> Parser::parse_rtype() {
    auto inst = std::make_unique<RTypeInst>();
    
    // mnemonic
    expect_token(TokenType::INST, "Expected instruction mnemonic");
    inst->mnemonic = current_token_.literal;
    advance();
    
    inst->rd = expect_register("Expected destination register for R-type instruction");
    
    // comma
    expect_token(TokenType::COMMA, "Expected comma after destination register");
    advance();
    
    inst->rs = expect_register("Expected first source register");
    
    // expect comma
    expect_token(TokenType::COMMA, "Expected comma after first source register");
    advance();
    
    // parse rt - for shifts, allow immediate shift amount
    if ((inst->mnemonic == "sll" || inst->mnemonic == "srl") && 
        current_token_.type == TokenType::NUMBER) {
        // immediate shift amount
        inst->rt = current_token_.literal;
        advance();
    } else {
        inst->rt = expect_register("Expected second source register or shift amount");
    }
    
    return inst;
}

std::unique_ptr<ITypeInst> Parser::parse_itype() {
    auto inst = std::make_unique<ITypeInst>();
    
    // mnemonic
    expect_token(TokenType::INST, "Expected instruction mnemonic");
    inst->mnemonic = current_token_.literal;
    advance();
    
    if (inst->mnemonic == "beq") {
        // beq rs, rt, label
        inst->rs = expect_register("Expected first register for beq");
        expect_token(TokenType::COMMA, "Expected comma after first register");
        advance();
        inst->rt = expect_register("Expected second register for beq");
        expect_token(TokenType::COMMA, "Expected comma after second register");
        advance();
        // label reference
        expect_token(TokenType::IDENT, "Expected label name for beq");
        inst->imm_or_label = current_token_.literal;
        inst->is_label_ref = true;
        advance();
    } else if (inst->mnemonic == "lw" || inst->mnemonic == "sw") {
        // lw/sw rt, imm(rs) - lw/sw rt, label - lw/sw rt, label(rs)
        inst->rt = expect_register("Expected target register for lw/sw");
        expect_token(TokenType::COMMA, "Expected comma after target register");
        advance();
        
        // label or immediate value
        if (current_token_.type == TokenType::IDENT) {
            // label
            inst->imm_or_label = current_token_.literal;
            inst->is_label_ref = true;
            advance();
            
            if (current_token_.type == TokenType::LPAREN) {
                advance();
                inst->rs = expect_register("Expected source register in parentheses");
                expect_token(TokenType::RPAREN, "Expected closing parenthesis");
                advance();
            }
        } else {
            // immediate
            expect_token(TokenType::NUMBER, "Expected immediate value or label");
            inst->imm_or_label = current_token_.literal;
            inst->is_label_ref = false;
            advance();
            
            if (current_token_.type == TokenType::LPAREN) {
                advance();
                inst->rs = expect_register("Expected source register in parentheses");
                expect_token(TokenType::RPAREN, "Expected closing parenthesis");
                advance();
            }
        }
    }
    
    return inst;
}

std::unique_ptr<DirectiveNode> Parser::parse_directive() {
    auto dir = std::make_unique<DirectiveNode>();
    
    expect_token(TokenType::DOT, "Expected '.' for directive");
    advance();
    
    // directive name
    expect_token(TokenType::IDENT, "Expected directive name after '.'");
    dir->name = current_token_.literal;
    advance();
    
    if (current_token_.type == TokenType::COLON) {
        advance();
    }
    
    // .word values if applicable
    if (dir->name == "word") {
        // parse a comma-separated list of numbers or identifiers
        bool expect_value = true;
        while (true) {
            if (expect_value) {
                if (current_token_.type == TokenType::NUMBER || current_token_.type == TokenType::IDENT) {
                    dir->values.push_back(current_token_.literal);
                    advance();
                    expect_value = false;
                } else {
                    break;
                }
            } else {
                if (current_token_.type == TokenType::COMMA) {
                    advance();
                    expect_value = true;
                    continue;
                } else {
                    break;
                }
            }
        }
    }
    
    return dir;
}

std::unique_ptr<LabelNode> Parser::parse_label() {
    auto label = std::make_unique<LabelNode>();
    
    expect_token(TokenType::IDENT, "Expected label name");
    label->name = current_token_.literal;
    advance();
    
    expect_token(TokenType::COLON, "Expected ':' after label name");
    advance();
    
    return label;
}

std::unique_ptr<Node> Parser::parse_statement() {
    // directive
    if (current_token_.type == TokenType::DOT) {
        return parse_directive();
    }
    
    // ident
    if (current_token_.type == TokenType::IDENT && next_token_.type == TokenType::COLON) {
        auto label = std::make_unique<LabelNode>();
        label->name = current_token_.literal;
        advance(); advance();
        return label;
    }
    
    // instruction
    if (current_token_.type == TokenType::INST) {
        const auto mnemonic = current_token_.literal;
        if (is_rtype_instruction(mnemonic)) {
            return parse_rtype();
        } else if (is_itype_instruction(mnemonic)) {
            return parse_itype();
        } else {
            throw std::runtime_error("Unknown instruction: " + mnemonic);
        }
    }
    
    // Skip illegal
    if (current_token_.type == TokenType::ILLEGAL) {
        advance();
        return nullptr;
    }
    
    std::ostringstream oss;
    oss << "Unexpected token in statement. Type: " << static_cast<int>(current_token_.type)
        << ", literal: '" << current_token_.literal << "', line: " << current_token_.line;
    throw std::runtime_error(oss.str());
}

AST Parser::parse() {
    AST ast;
    
    while (current_token_.type != TokenType::EoF) {
        try {
            auto node = parse_statement();
            if (node) {
                ast.nodes.push_back(std::move(node));
            }
        } catch (const std::exception& e) {
            // try to recover from errors
            if (current_token_.type == TokenType::ILLEGAL) {
                advance();
                continue;
            }
            throw;
        }
    }
    
    return ast;
}

