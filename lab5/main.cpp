#include "Header.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif
 
static void set_utf8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

int main(int argc, char* argv[]) {
    set_utf8();
    bool compress = false;
    bool decompress = false;
    bool reset = false;
    uint32_t max_bits = lzw_def;
    std::string input, output;

    for (int i = 1; i < argc; ) {
        std::string a = argv[i];
        if(a == "-c") { compress = true; ++i; }
        else if(a == "-d") { decompress = true; ++i; }
        else if(a == "-r") { reset = true; ++i; }
        else if(a == "-b") {
            if (i + 1 >= argc) { 
                std::cerr << "-b треба аргумент\n"; 
                return 1; 
            }
            max_bits = (uint32_t)atoi(argv[i + 1]);
            i += 2;
        } else {
            if(input.empty())  input  = argv[i++];
            else if(output.empty()) output = argv[i++];
            else { 
                std::cerr << "лєвий аргумент: " << argv[i] << "\n"; 
                return 1; 
            }
        }
    }

    if (compress == decompress) {
        std::cerr << "-c чи -d не чує баба\n";
    }
    if (input.empty() || output.empty()) {
        std::cerr << "ввід вивід треба\n";
    }

    try {
        if (compress) {
            lzw_compr(input, output, max_bits, reset);
            std::cout << "Скомпрєсіровано: " << input << ", bits: " << max_bits << "\n";
        } else {
            lzw_decompr(input, output);
            std::cout << "Дєкомпрєсіровано: " << input << "\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "ошибка жизні: " << ex.what();
        return 2;
    }
    return 0;
}