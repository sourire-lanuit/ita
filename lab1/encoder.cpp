#include "Header.h"

namespace Base64 {

std::string encode(const std::vector<unsigned char>& data) {
    std::string out;
    size_t i = 0;

    while (i < data.size()) {
        unsigned int triple = 0;
        int bytes = 0;

        for (int j = 0; j < 3; ++j) {
            triple <<= 8;
            if (i < data.size()) {
                triple |= data[i++];
                bytes++;
            }
        }

        for (int j = 0; j < 4; ++j) {
            if (j <= bytes) {
                int index = (triple >> (18 - 6 * j)) & 0x3F;
                out += alphabet[index];
            } else {
                out += padding;
            }
        }
    }
    return out;
}
}