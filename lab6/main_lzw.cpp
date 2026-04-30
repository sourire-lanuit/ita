#include "Header.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
static void set_utf8() { SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); }
#else
static void set_utf8() {}
#endif

int main(int argc, char* argv[]) {
    set_utf8();
    bool do_compress = false, do_decompress = false, reset = false;
    uint32_t max_bits = lzw_def;
    std::string input, output;
    trans_opt opt;
 
    for (int i = 1; i < argc; ) {
        std::string a = argv[i];
        if (a == "-c") { do_compress = true; ++i; }
        else if (a == "-d") { do_decompress = true; ++i; }
        else if (a == "-r")  { reset = true; ++i; }
        else if (a == "--bwt") { opt.use_bwt  = true; ++i; }
        else if (a == "--mtf") { opt.use_mtf = true; ++i; }
        else if (a == "-b") {
            if (i + 1 >= argc) { std::cerr << "-b треба аргумент\n"; return 1; }
            max_bits = static_cast<uint32_t>(std::atoi(argv[i + 1]));
            if (max_bits < lzw_min || max_bits > lzw_max) {
                std::cerr << "-b: від " << lzw_min << " до " << lzw_max << "\n";
                return 1;
            }
            i += 2;
        }
        else if (input.empty())  { input  = argv[i++]; }
        else if (output.empty()) { output = argv[i++]; }
        else { std::cerr << "це було лішнеє: " << a << "\n"; return 1; }
    }
 
    if (do_compress == do_decompress) {
        std::cerr << "-c чи -d не чує баба\n\n"; return 1;
    }
    if (input.empty() || output.empty()) {
        std::cerr << "Вкажіть вхідний-вихідний файл\n\n"; return 1;
    }
 
    try {
        if (do_compress) {
            lzw_compr_ex(input, output, opt, max_bits, reset);
            std::cout << "Стиснено: " << input << " -> " << output << "  bits = " << max_bits;
            if (reset) std::cout << " (reset)";
            if (opt.use_bwt) std::cout << " (BWT)";
            if (opt.use_mtf) std::cout << " (MTF)";
            std::cout << "\n";
        } else {
            lzw_decompr_ex(input, output);
            std::cout << "Розпаковано: " << input << " -> " << output << "\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "Помилка: " << ex.what() << "\n"; return 2;
    }
    return 0;
}