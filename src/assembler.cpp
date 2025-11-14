#include "assembler.h"
#include "code_gen.h"
#include "utils.h"

#include <iomanip>
#include <utility>


Assembler::Assembler(std::string  input) : input_(std::move(input)) {}

void Assembler::assemble(
    const std::string& instruction_file_path, const std::string& data_file_path,
    const std::string& instruction_template_path, const std::string& data_template_path
) const {
    Lexer lexer(input_);
    Parser parser(lexer);
    AST ast = parser.parse();

    CodeGenerator code_gen;
    const auto sym_table = code_gen.pass1(ast);
    const auto [instructions, data] = code_gen.pass2(ast, sym_table);

    utils::replace_marker_with_output(
        instruction_template_path, instruction_file_path,
        subs_token.data(), instructions
    );
    utils::replace_marker_with_output(
        data_template_path, data_file_path,
        subs_token.data(), data
    );
}
