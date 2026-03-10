#include "Header.h"

#include <iostream>

static void print(const char* prog)
{
    std::cerr << "Huffman compressor\n\n" << "Usage:\n" << "  " << prog << " -c <input>  <output>   compress\n" << "  " << prog << " -d <input>  <output>   decompress\n\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        print(argv[0]);
        return 1;
    }

    const std::string mode = argv[1];
    const std::string file_in  = argv[2];
    const std::string file_out = argv[3];

    bool ok = false;
    if (mode == "-c") ok = compress(file_in, file_out);
    else if (mode == "-d") ok = decompress(file_in, file_out);
    else {
        std::cerr << "Unknown mode: " << mode << "\n\n";
        print(argv[0]);
        return 1;
    }

    return ok ? 0 : 1;
}