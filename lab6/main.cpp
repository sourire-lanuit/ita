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

    std::string mode, file_in, file_out;
    trans_opt opt;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-c" || a == "-d") mode = a;
        else if (a == "--bwt") opt.use_bwt = true;
        else if (a == "--mtf") opt.use_mtf = true;
        else if (file_in.empty()) file_in = a;
        else if (file_out.empty()) file_out = a;
        else { std::cerr << "шо це і аткуда?: " << a << "\n"; return 1; }
    }

    if (mode.empty() || file_in.empty() || file_out.empty()) return 1;

    bool ok = false;
    if (mode == "-c") {
        ok = compress_ex(file_in, file_out, opt);
        if (ok) {
            std::cout << "Стиснено: " << file_in << " -> " << file_out;
            if (opt.use_bwt) std::cout << " (BWT)";
            if (opt.use_mtf) std::cout << " (MTF)";
            std::cout << "\n";
        }
    } else {
        ok = decompress_ex(file_in, file_out);
        if (ok) std::cout << "Розпаковано: " << file_in << " -> " << file_out << "\n";
    }
    return ok ? 0 : 1;

    return 0;
}