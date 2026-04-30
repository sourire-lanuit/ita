#include "Header.h"

#include <iostream>
#include <stdexcept>
#include <array>

static void write32(std::vector<uint8_t>& buf, uint32_t v) {
    buf.push_back(static_cast<uint8_t>(v & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
}

static void write64(std::vector<uint8_t>& buf, uint64_t v) {
    for (int i = 0; i < 8; ++i) buf.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xFF));
}

bool compress(const std::string& file_in, const std::string& file_out) {
    std::ifstream fin(file_in, std::ios::binary);
    if (!fin) {
        std::cerr << "Entry error: " << file_in << "\n";
        return false;
    }
    std::vector<uint8_t> input((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));
    fin.close();

    const uint64_t original = input.size();
    std::array<uint32_t, alph_size> freqs{};
    for (uint8_t b : input) ++freqs[b];

    Huffman* root = build_tree(freqs);
    the_table table;
    build_table(root, table);
    free_tree(root);

    std::vector<uint8_t> outbuf;
    outbuf.reserve(table_size + 8 + input.size());

    for (int i = 0; i < alph_size; ++i) write32(outbuf, freqs[i]);
    write64(outbuf, original);

    uint8_t wbuf = 0;
    int wpos = 0;
    auto flush_bit = [&](int bit) {
        if (bit) wbuf |= static_cast<uint8_t>(1 << (7 - wpos));
        if (++wpos == 8) { outbuf.push_back(wbuf); wbuf = 0; wpos = 0; }
    };
    for (uint8_t b : input)
        for (int i = 0; i < table[b].length; ++i) flush_bit((table[b].bits[i / 8] >> (i % 8)) & 1);
    if (wpos > 0) outbuf.push_back(wbuf);

    std::ofstream fout(file_out, std::ios::binary);
    if (!fout) {
        std::cerr << "Output error: " << file_out << "\n";
        return false;
    }
    fout.write(reinterpret_cast<const char*>(outbuf.data()), static_cast<std::streamsize>(outbuf.size()));

    uint64_t out_size = outbuf.size();
    std::cerr << "Compressed file: " << original << out_size << " bytes  (in ratio " << (original ? (100.0 * out_size / original) : 0.0) << "%)\n";
    return true;
}