#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <fstream>
#include <exception>

namespace Base64 {
    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    constexpr char padding = '=';
    constexpr char comment = '-';
    constexpr size_t line_lenght = 76;

    std::vector<unsigned char> read_file(const std::string& filename);
    void write_file(const std::string& filename, const std::vector<unsigned char>& data);

    std::vector<std::string> read_text(const std::string& filename);
    void write_text(const std::string& filename, const std::vector<std::string>& lines);

    std::string encode(const std::vector<unsigned char>& data);
    std::vector<unsigned char> decode(const std::vector<std::string>& lines);

    int alph_index(char c);
}

#endif 