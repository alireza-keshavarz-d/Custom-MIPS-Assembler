#include "lexer.h"
#include <cctype>


Lexer::Lexer(const std::string& input)
    : input_(input), pos_(0), line_(1), current_char_(input.empty() ? '\0' : input[0]) {}

void Lexer::advance() {
    if (pos_ + 1 < input_.size()) {
        pos_++;
        current_char_ = input_[pos_];
        if (current_char_ == '\n') line_++;
    } else {
        current_char_ = '\0';
    }
}

void Lexer::skip_whitespace() {
    while (current_char_ != '\0' && std::isspace(current_char_)) {
        advance();
    }
}

void Lexer::skip_comment() {
    while (current_char_ != '\0' && current_char_ != '\n') {
        advance();
    }
}

std::string Lexer::read_ident() {
    auto result = std::string{};
    while (current_char_ != '\0' && (std::isalnum(current_char_) || current_char_ == '_')) {
        result += current_char_;
        advance();
    }
    return result;
}

std::string Lexer::read_number() {
    auto result = std::string{};
    if (current_char_ == '-') {
        result += '-';
        advance();
    }
    if (current_char_ == '0' && (std::tolower(input_[pos_ + 1]) == 'x')) {
        result += "0x";
        advance(); advance(); // skip 0x
        while (std::isxdigit(current_char_)) {
            result += current_char_;
            advance();
        }
    } else {
        while (std::isdigit(current_char_)) {
            result += current_char_;
            advance();
        }
    }
    return result;
}

bool Lexer::is_instruction(const std::string& s) const {
    return INSTRUCTIONS.contains(s);
}

bool Lexer::is_register(const std::string& s) const {
    auto reg = s;
    if (!reg.empty() && reg[0] == '$') reg = reg.substr(1);
    return REGISTERS.contains(reg);
}

Token Lexer::next_token() {
    skip_whitespace();

    if (current_char_ == ';') {
        skip_comment();
        return next_token();
    }

    if (current_char_ == '\0') {
        return {TokenType::EoF, "", line_};
    }

    if (std::isalpha(current_char_) || current_char_ == '_') {
        auto ident = read_ident();
        if (is_instruction(ident)) {
            return {TokenType::INST, ident, line_};
        }
        return {TokenType::IDENT, ident, line_};
    }

    if (std::isdigit(current_char_) || current_char_ == '-' || (current_char_ == '0' && std::tolower(input_[pos_ + 1]) == 'x')) {
        return {TokenType::NUMBER, read_number(), line_};
    }

    if (current_char_ == '$') {
        advance();
        const auto reg = read_ident();
        if (is_register(reg)) {
            return {TokenType::REGISTER, "$" + reg, line_};
        }
        return {TokenType::ILLEGAL, "$" + reg, line_};
    }

    if (current_char_ == '.') {
        advance();
        return {TokenType::DOT, ".", line_};
    }

    if (current_char_ == ',') {
        advance();
        return {TokenType::COMMA, ",", line_};
    }

    if (current_char_ == ':') {
        advance();
        return {TokenType::COLON, ":", line_};
    }

    if (current_char_ == '(') {
        advance();
        return {TokenType::LPAREN, "(", line_};
    }

    if (current_char_ == ')') {
        advance();
        return {TokenType::RPAREN, ")", line_};
    }

    // no match --> illegal
    const auto illegal = std::string(1, current_char_);
    advance();
    return {TokenType::ILLEGAL, illegal, line_};
}
