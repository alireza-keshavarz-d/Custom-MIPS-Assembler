#ifndef LEXER_H
#define LEXER_H


#include "common.h"

#include <unordered_set>


class Lexer {
public:
    explicit Lexer(const std::string& input);

    Token next_token();

private:
    void advance();
    void skip_whitespace();
    void skip_comment();
    std::string read_ident();
    std::string read_number();
    [[nodiscard]] bool is_instruction(const std::string& s) const;
    [[nodiscard]] bool is_register(const std::string& s) const;

    std::string input_;
    size_t pos_;
    int line_;
    char current_char_;
};

#endif // LEXER_H