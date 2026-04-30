#include "Header.h"
#include <cstring>

static std::vector<uint8_t> pack(uint32_t code, uint32_t width) {
    int byte_count = ((int)width + 7) / 8;
    std::vector<uint8_t> buf(byte_count, 0);
    for (uint32_t i = 0; i < width; ++i) {
        if ((code >> i) & 1u) buf[i / 8] |= (uint8_t)(1 << (i % 8));
    }
    return buf;
}

static uint32_t unpack(const std::vector<uint8_t>& buf, uint32_t width) {
    uint32_t code = 0;
    for (uint32_t i = 0; i < width; ++i) {
        int byte_ind = (int)i / 8;
        int bit_ind  = (int)i % 8;
        if ((buf[byte_ind] >> bit_ind) & 1) code |= (1u << i);
    }
    return code;
}

static void write_u32(std::ofstream& f, uint32_t v) {
    uint8_t b[4] = { (uint8_t)(v & 0xFF), (uint8_t)((v >> 8) & 0xFF), (uint8_t)((v >> 16) & 0xFF), (uint8_t)((v >> 24) & 0xFF) };
    f.write(reinterpret_cast<char*>(b), 4);
}

static uint32_t read_u32(std::ifstream& f) {
    uint8_t b[4];
    f.read(reinterpret_cast<char*>(b), 4);
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

void lzw_compr(const std::string& src_file, const std::string& dst_file, uint32_t max_bits, bool reset) {
    if (max_bits < lzw_min || max_bits > lzw_max) throw std::invalid_argument("max_bits 9-16");

    std::ifstream src(src_file, std::ios::binary | std::ios::ate);
    if (!src) throw std::runtime_error("не відкривсі: " + src_file);
    uint32_t src_size = (uint32_t)src.tellg();
    src.seekg(0);

    BSt_Writer bw;
    if (!o_writer(bw, dst_file)) throw std::runtime_error("не створивсі: " + dst_file);
    bw.file.write(reinterpret_cast<const char*>(lzw), 4);
    bw.file.put((char)max_bits);
    bw.file.put((char)(reset ? 1 : 0));
    write_u32(bw.file, src_size);

    CompressDict dict(max_bits);

    if (src_size == 0) { x_writer(bw); return; }
    int first = src.get();
    if (first == EOF) { x_writer(bw); return; }
    dict.restart((uint8_t)first);

    int ch;
    while ((ch = src.get()) != EOF) {
        uint8_t b = (uint8_t)ch;
        if (!dict.step(b)) {
            uint32_t width = dict.current_bits();
            auto buf = pack(dict.current_code(), width);
            write_bs(bw, buf, (int)width);

            if (dict.is_full()) {
                if (reset) dict.reset();
            }
            dict.add_phrase(b);
            dict.restart(b);
        }
    }
    {
        uint32_t width = dict.current_bits();
        auto buf = pack(dict.current_code(), width);
        write_bs(bw, buf, (int)width);
    }

    x_writer(bw);
    src.close();
}

void lzw_decompr(const std::string& src_file, const std::string& dst_file) {
    BSt_Reader br;
    if (!o_reader(br, src_file)) throw std::runtime_error("кент опен: " + src_file);

    uint8_t magic[4];
    br.file.read(reinterpret_cast<char*>(magic), 4);
    if (memcmp(magic, lzw, 4) != 0) {
        x_reader(br);
        throw std::runtime_error("файл невалідний");
    }

    int mb = br.file.get();
    if (mb == EOF) { x_reader(br); throw std::runtime_error("заголовок в мінусє"); }
    uint32_t max_bits = (uint32_t)(uint8_t)mb;
    int rf = br.file.get();
    if (rf == EOF) { x_reader(br); throw std::runtime_error("заголовок в мінусє"); }
    bool reset = (rf != 0);

    uint32_t orig_size = read_u32(br.file);
    std::ofstream dst(dst_file, std::ios::binary);
    if (!dst) { x_reader(br); throw std::runtime_error("Створенню не підлягає: " + dst_file); }

    DecompressDict dict(max_bits);
    uint32_t written = 0;

    if (orig_size == 0) { x_reader(br); return; }

    auto read_code = [&](uint32_t width) -> uint32_t {
        std::vector<uint8_t> buf;
        read_bs(br, buf, (int)width);
        return unpack(buf, width);
    };

    auto emit = [&](const std::vector<uint8_t>& s) {
        for (uint8_t byte : s) if (written < orig_size) { dst.put((char)byte); ++written; }
    };

    uint32_t prev_code = read_code(dict.current_bits());
    emit(dict.decode(prev_code));

    while (written < orig_size) {
        uint32_t width    = dict.current_bits();
        uint32_t cur_code = read_code(width);

        if (cur_code > dict.next_code()) {
            x_reader(br);
            throw std::runtime_error("декомпресія не спрацювала " + std::to_string(cur_code));
        }

        if (!dict.is_full()) {
            dict.add_entry(prev_code, cur_code);
        } else if (reset) {
            dict.reset();
        }

        emit(dict.decode(cur_code));
        prev_code = cur_code;
    }

    x_reader(br);
    dst.close();
}