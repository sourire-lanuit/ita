#ifndef HEADER_H
#define HEADER_H

#include <string>

namespace RLE {
    const std::string form = "RLE";
    bool encode(const std::string& input, const std::string& output);
    bool decode(const std::string& input, const std::string& output);
}

#endif