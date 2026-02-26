#include "Header.h"

bool o_reader(BSt_Reader& bs, const std::string& file_name) {
    bs.file.open(file_name, std::ios::binary);
    bs.pos = 8;
    return bs.file.is_open();
}

void x_reader(BSt_Reader& bs) {
    bs.file.close();
}

static int read(BSt_Reader& bs) {
    if (bs.pos == 8) {
        int c = bs.file.get();
        if (c == EOF) return 0;
        bs.buffer = static_cast<uint8_t>(c);
        bs.pos = 0;
    }

    int bit = (bs.buffer >> (7 - bs.pos)) & 1;
    bs.pos++;

    return bit;
}

void read_bs(BSt_Reader& bs, std::vector<uint8_t>& data, int count) {
    int byte_count = (count + 7) / 8;
    data.assign(byte_count, 0);
    for (int i = 0; i < count; i++) {
        int bit = read(bs);
        int byte_ind = i / 8;
        int bit_ind = i % 8;        
        if (bit) data[byte_ind] |= (1 << bit_ind);
    }
}