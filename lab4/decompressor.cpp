#include "Header.h"

#include <iostream>
#include <array>

static uint32_t read32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) <<  8) | (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

static uint64_t read64(const uint8_t* p) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v |= static_cast<uint64_t>(p[i]) << (8 * i);
    return v;
}

static constexpr std::size_t h_size = table_size + 8; 

bool decompress(const std::string& file_in, const std::string& file_out) {
    std::ifstream fhdr(file_in, std::ios::binary);
    if (!fhdr) {
        std::cerr << "Помилка відкриття стиснутого файлу: " << file_in << "\n";
        return false;
    }

    uint8_t hdr(h_size);
    if (!fhdr.read(reinterpret_cast<char*>(hdr.data()), static_cast<std::streamsize>(h_size))) {
        std::cerr << "Файл замалий архівації\n";
        return false;
    }
    fhdr.close();

    std::array<uint32_t, alph_size> freqs{};
    for (int i = 0; i < alph_size; ++i)
        freqs[i] = read32(hdr.data() + i * 4);

    const uint64_t original = read64(hdr.data() + table_size);

    Huffman* root = build_tree(freqs);

    if (original == 0) {
        free_tree(root);
        std::ofstream fout(file_out, std::ios::binary);
        return static_cast<bool>(fout);
    }

    if (!root) {
        std::cerr << "Дерево перебудові не підлягає\n";
        return false;
    }

    BSt_Reader bsr;
    if (!o_reader(bsr, file_in)) {
        std::cerr << "Помилка відкриття стиснутого файлу для читання: " << file_in << "\n";
        free_tree(root);
        return false;
    }
    bsr.file.seekg(static_cast<std::streamoff>(h_size), std::ios::beg);
    bsr.pos = 8; 

    std::vector<uint8_t> output;
    output.reserve(static_cast<std::size_t>(original));

    const bool ssymbol = (!root->left || !root->right);

    for (uint64_t decoded = 0; decoded < original; ++decoded) {
        if (ssymbol) {
            std::vector<uint8_t> dummy;
            read_bs(bsr, dummy, 1);
            Huffman* leaf = root->left ? root->left : root;
            output.push_back(static_cast<uint8_t>(leaf->symbol));
        } else {
            Huffman* node = root;
            while (node->left || node->right) {
                std::vector<uint8_t> bit_vec;
                read_bs(bsr, bit_vec, 1);
                int bit = bit_vec[0] & 1;
                node = (bit == 0) ? node->left : node->right;
                if (!node) {
                    std::cerr << "Потік утік (досягнуто нульове значення)\n";
                    free_tree(root);
                    return false;
                }
            }
            output.push_back(static_cast<uint8_t>(node->symbol));
        }
    }

    x_reader(bsr);
    free_tree(root);

    std::ofstream fout(file_out, std::ios::binary);
    if (!fout) {
        std::cerr << "Помилка виводу: " << file_out << "\n";
        return false;
    }
    fout.write(reinterpret_cast<const char*>(output.data()), static_cast<std::streamsize>(output.size()));

    std::cerr << "Decompressed file: " << (original + h_size) << output.size() << " bytes\n";
    return true;
}