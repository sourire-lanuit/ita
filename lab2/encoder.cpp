#include "Header.h"
#include <fstream>
#include <vector>

bool RLE::encode(const std::string& input, const std::string& output) {
    std::ifstream in(input, std::ios::binary);
    if (!in) return false;
    std::ofstream out(output, std::ios::binary);
    if (!out) return false;

    out.write(form.c_str(), form.size());

    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    size_t i = 0;

    while (i < buffer.size()) {
        size_t run_length = 1;
        while (i + run_length < buffer.size() && buffer[i] == buffer[i + run_length] && run_length <= 128) {
            run_length++;
        }

        if (run_length >= 2) {
            unsigned char L = 128 + (run_length - 2);
            out.put(L);
            out.put(buffer[i]);
            i += run_length;
        }

        else {
            size_t start = i;
            run_length = 1;

            while (i + run_length < buffer.size()) {
                if (buffer[i + run_length] == buffer[i + run_length - 1]) break;
                if (run_length >= 128) break;
                run_length++;
            }

            unsigned char L = run_length - 1;
            out.put(L);
            for (size_t j = 0; j < run_length; j++) out.put(buffer[i + j]);
            i += run_length;
        }
    }

    return true;
}