#ifndef HEADER_H
#define HEADER_H

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <array>

static constexpr int alph_size = 256;
static constexpr int table_size = alph_size * 4; 

struct BSt_Writer {
    std::ofstream file;
    uint8_t buffer = 0;
    int pos = 0; 
};

struct BSt_Reader {
    std::ifstream file;
    uint8_t buffer = 0;
    int pos = 8; 
};

bool o_writer(BSt_Writer& bs, const std::string& file_name);
void x_writer(BSt_Writer& bs);
void write_bs(BSt_Writer& bs, const std::vector<uint8_t>& data, int count);
bool o_reader(BSt_Reader& bs, const std::string& file_name);
void x_reader(BSt_Reader& bs);
void read_bs(BSt_Reader& bs, std::vector<uint8_t>& data, int count);

struct Huffman {
    uint64_t freq;  
    int symbol;
    Huffman* left;
    Huffman* right;
    Huffman(uint64_t f, int sym, Huffman* l = nullptr, Huffman* r = nullptr) : freq(f), symbol(sym), left(l), right(r) {}
};

bool compress(const std::string& file_in, const std::string& file_out);
bool decompress(const std::string& file_in, const std::string& file_out);

struct Structura {
    std::vector<uint8_t> bits; 
    int length = 0;
};

using the_table = std::array<Structura, alph_size>;
Huffman* build_tree(const std::array<uint32_t, alph_size>& freqs);
void free_tree(Huffman* root);
void build_table(Huffman* root, the_table& table);

#endif