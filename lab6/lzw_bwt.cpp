#include "Header.h"

void lzw_compr_ex(const std::string& src_file, const std::string& dst_file, const trans_opt& opt, uint32_t max_bits, bool reset_dict) {
    std::vector<uint8_t> input = read_bytes(src_file);

    uint32_t bwt_idx = 0;
    std::vector<uint8_t> transformed = apply_trans(input, opt, bwt_idx);

    const std::string tmp_in  = dst_file + "._t1";
    const std::string tmp_out = dst_file + "._t2";
    write_bytes(tmp_in, transformed);
    lzw_compr(tmp_in, tmp_out, max_bits, reset_dict);
    std::remove(tmp_in.c_str());

    std::vector<uint8_t> compressed = read_bytes(tmp_out);
    std::remove(tmp_out.c_str());

    std::ofstream fout(dst_file, std::ios::binary);
    if (!fout) throw std::runtime_error("помилка, шо ти пишеш?: " + dst_file);

    uint8_t flags = 0;
    if (opt.use_bwt) flags |= FLAG_BWT;
    if (opt.use_mtf) flags |= FLAG_MTF;
    fout.put(static_cast<char>(flags));
    if (opt.use_bwt) {
        fout.put(static_cast<char>( bwt_idx & 0xFF));
        fout.put(static_cast<char>((bwt_idx >> 8) & 0xFF));
        fout.put(static_cast<char>((bwt_idx >> 16) & 0xFF));
        fout.put(static_cast<char>((bwt_idx >> 24) & 0xFF));
    }
    fout.write(reinterpret_cast<const char*>(compressed.data()), static_cast<std::streamsize>(compressed.size()));
}

void lzw_decompr_ex(const std::string& src_file, const std::string& dst_file) {
    std::vector<uint8_t> raw = read_bytes(src_file);
    if (raw.empty()) throw std::runtime_error("пусто");

    size_t pos = 0;
    uint8_t flags = raw[pos++];
    trans_opt opt;
    opt.use_bwt = (flags & FLAG_BWT) != 0;
    opt.use_mtf = (flags & FLAG_MTF) != 0;

    uint32_t bwt_idx = 0;
    if (opt.use_bwt) {
        if (raw.size() < pos + 4) throw std::runtime_error("Назва в тур не поїхала?");
        bwt_idx = static_cast<uint32_t>(raw[pos]) | (static_cast<uint32_t>(raw[pos+1]) << 8) | (static_cast<uint32_t>(raw[pos+2]) << 16) | (static_cast<uint32_t>(raw[pos+3]) << 24);
        pos += 4;
    }

    const std::string tmp_in  = src_file + "._t1";
    const std::string tmp_out = dst_file + "._t2";
    write_bytes(tmp_in, std::vector<uint8_t>(raw.begin() + static_cast<ptrdiff_t>(pos), raw.end()));
    lzw_decompr(tmp_in, tmp_out);
    std::remove(tmp_in.c_str());
    std::vector<uint8_t> expanded = read_bytes(tmp_out);
    std::remove(tmp_out.c_str());
    write_bytes(dst_file, undo_trans(expanded, opt, bwt_idx));
}