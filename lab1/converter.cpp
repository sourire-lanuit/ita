#include "Header.h"

namespace Base64 {
std::vector<unsigned char> read_file(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Unable to open input file");
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void write_file(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) throw std::runtime_error("Unable to open output file");
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
}

std::vector<std::string> read_text(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Unable to open input file");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);

    return lines;
}

void write_text(const std::string& filename, const std::vector<std::string>& lines) {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Unable to open output file");
    for (const auto& l : lines) out << l << '\n';
}
}