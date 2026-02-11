#include "Header.h"

namespace Base64 {
int alph_index(char c) {
    auto position = alphabet.find(c);
    return position == std::string::npos ? -1 : static_cast<int>(position);
}

std::vector<unsigned char> decode(const std::vector<std::string>& lines) {
    std::vector<unsigned char> result;
    bool finished = false;

    for (size_t ln = 0; ln < lines.size(); ++ln) {
        const auto& line = lines[ln];

        if (line.empty()) continue;

        if (line[0] == comment) continue;

        if (finished) throw std::runtime_error("Error: data after the end of message");

        if (line.size() != line_lenght && ln + 1 != lines.size()) throw std::runtime_error("Line " + std::to_string(ln + 1) + ": invalid line length (" + std::to_string(line.size()) + ")");

        unsigned int buffer = 0;
        int bits = 0;
        int padding_count = 0;

        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];

            if (c == padding) {
                padding_count++;
                buffer <<= 6;
            } else {
                int val = alph_index(c);
                if (val < 0) throw std::runtime_error("Line " + std::to_string(ln + 1) + ", char " + std::to_string(i + 1) +": invalid character");
                buffer = (buffer << 6) | val;
            }

            bits += 6;

            if (bits >= 8) {
                bits -= 8;
                unsigned char byte = (buffer >> bits) & 0xFF;
                if (padding_count == 0) result.push_back(byte);
            }
        }

        if (padding_count > 0) finished = true;
    }

    return result;
}
}