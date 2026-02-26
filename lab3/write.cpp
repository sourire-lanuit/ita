#include "Header.h"

bool o_writer(BSt_Writer& bs, const std::string& file_name) {
    bs.file.open(file_name, std::ios::binary);
    bs.buffer = 0;
    bs.pos = 0;
    return bs.file.is_open();
}

void x_writer(BSt_Writer& bs) {
    if (bs.pos > 0) bs.file.put(bs.buffer);
    bs.file.close();
}

static void write(BSt_Writer& bs, int bit) {
    if (bit) bs.buffer |= (1 << (7 - bs.pos));
    bs.pos++;
    if (bs.pos == 8) {
        bs.file.put(bs.buffer);
        bs.buffer = 0;
        bs.pos = 0;
    }
}

void write_bs(BSt_Writer& bs, const std::vector<uint8_t>& data, int count) {
    for (int i = 0; i < count; i++) {
        int byte_ind = i / 8;
        int bit_ind = i % 8;
        int bit = (data[byte_ind] >> bit_ind) & 1;
        write(bs, bit);
    }
}