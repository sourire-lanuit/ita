#include "Header.h"
#include <fstream>

bool RLE::decode(const std::string& input, const std::string& output) {
    std::ifstream in(input, std::ios::binary);
    if (!in) return false;

    std::ofstream out(output, std::ios::binary);
    if (!out) return false;

    char rle[3];
    in.read(rle, 3);

    if (std::string(rle, 3) != form) return false;

    while (true) {
        int L = in.get();
        if (L == EOF) break;

        if (L & 0x80) {
            int length = (L - 128) + 2;
            int value = in.get();
            if (value == EOF) return false;
            for (int i = 0; i < length; i++) out.put(static_cast<unsigned char>(value));
        }
        else {
            int length = L + 1;
            for (int i = 0; i < length; i++) {
                int value = in.get();
                if (value == EOF) return false;
                out.put(static_cast<unsigned char>(value));
            }
        }
    }

    return true;
}