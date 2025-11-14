#ifndef UTILS_H
#define UTILS_H


#include <bitset>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


namespace utils {

inline void replace_marker_with_output(
    const std::string& template_file_path,
    const std::string& output_file_path,
    const std::string& subs_token,
    const std::vector<uint8_t>& data
) {
    std::ifstream in(template_file_path);
    if (!in) throw std::runtime_error("Failed to open template file: " + template_file_path);

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(in, line)) {
        lines.push_back(line);
    }
    in.close();

    std::vector<std::string> data_lines;
    data_lines.reserve(data.size() / 4);

    for (size_t i = 0, index = 0; i < data.size(); i += 4, ++index) {
        uint32_t word =
            (static_cast<uint32_t>(data[i])   << 24) |
            (static_cast<uint32_t>(data[i + 1]) << 16) |
            (static_cast<uint32_t>(data[i + 2]) <<  8) |
             static_cast<uint32_t>(data[i + 3]);
        data_lines.push_back(
            std::to_string(index) + " => \"" +
            std::bitset<32>(word).to_string() + "\","
        );
    }
    data_lines.emplace_back("others => (others => '0')\n");

    auto replaced = false;
    for (size_t idx = 0; idx < lines.size(); ++idx) {
        if (lines[idx].find(subs_token) != std::string::npos) {
            replaced = true;
            lines.erase(lines.begin() + idx);
            lines.insert(lines.begin() + idx, data_lines.begin(), data_lines.end());
            break;
        }
    }

    if (!replaced)
        throw std::runtime_error("No line starting with specified token found in file " + template_file_path);

    std::ofstream out(output_file_path, std::ios::trunc);
    if (!out) throw std::runtime_error("Failed to write to file: " + output_file_path);

    for (const auto& l : lines) {
        out << l << '\n';
    }
}

}


#endif //UTILS_H
