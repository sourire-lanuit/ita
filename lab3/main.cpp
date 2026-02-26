#include "Header.h"
#include <iostream>

int main() {
    BSt_Writer writer;
    if (!o_writer(writer, "data.bin")) {
        std::cout << "Failed to open a file for writing\n";
        return 1;
    }

    std::vector<uint8_t> a1 = {0xE1, 0x01}; 
    std::vector<uint8_t> a2 = {0xEE, 0x00}; 
    write_bs(writer, a1, 9);
    write_bs(writer, a2, 9);
    x_writer(writer);

    std::ifstream check("data.bin", std::ios::binary);
    std::cout << "file bytes: ";
    int chek;
    while ((chek = check.get()) != EOF) printf("%02X ", chek);
    check.close();

    BSt_Reader reader;
    if (!o_reader(reader, "data.bin")) {
        std::cout << "Failed to open a file for reading\n";
        return 1;
    }
    
    std::vector<uint8_t> b1, b2;
    read_bs(reader, b1, 11);
    read_bs(reader, b2, 7);
    x_reader(reader);

    std::cout << "\nb1: ";
    for (auto b: b1) printf("%02X ", b);
    std::cout << "\nb2: ";
    for (auto b: b2) printf("%02X ", b);

    return 0;
}