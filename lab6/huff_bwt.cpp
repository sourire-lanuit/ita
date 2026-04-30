#include "Header.h"

bool compress_ex(const std::string& file_in, const std::string& file_out, const trans_opt& opt) {
    std::vector<uint8_t> input;
    try { input = read_bytes(file_in); }
    catch (const std::exception& e) { std::cerr << e.what() << "\n"; return false; }

    uint32_t bwt_idx = 0;
    std::vector<uint8_t> transformed = apply_trans(input, opt, bwt_idx);

    const std::string tmp_in  = file_out + "._t1";
    const std::string tmp_out = file_out + "._t2";
    try { write_bytes(tmp_in, transformed); }
    catch (const std::exception& e) { std::cerr << e.what() << "\n"; return false; }

    if (!compress(tmp_in, tmp_out)) {
        std::remove(tmp_in.c_str());
        return false;
    }
    std::remove(tmp_in.c_str());

    std::vector<uint8_t> compressed;
    try { compressed = read_bytes(tmp_out); }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::remove(tmp_out.c_str());
        return false;
    }
    std::remove(tmp_out.c_str());

    std::ofstream fout(file_out, std::ios::binary);
    if (!fout) { std::cerr << "лікарю, воно не записується: " << file_out << "\n"; return false; }

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
    return true;
}

bool decompress_ex(const std::string& file_in, const std::string& file_out) {
    std::vector<uint8_t> raw;
    try { raw = read_bytes(file_in); }
    catch (const std::exception& e) { std::cerr << e.what() << "\n"; return false; }
    if (raw.empty()) { std::cerr << "і настала пустота\n"; return false; }

    size_t pos = 0;
    uint8_t flags = raw[pos++];
    trans_opt opt;
    opt.use_bwt = (flags & FLAG_BWT) != 0;
    opt.use_mtf = (flags & FLAG_MTF) != 0;

    uint32_t bwt_idx = 0;
    if (opt.use_bwt) {
        if (raw.size() < pos + 4) { std::cerr << "Назва в тур не їде?\n"; return false; }
        bwt_idx = static_cast<uint32_t>(raw[pos]) | (static_cast<uint32_t>(raw[pos+1]) << 8) | (static_cast<uint32_t>(raw[pos+2]) << 16) | (static_cast<uint32_t>(raw[pos+3]) << 24);
        pos += 4;
    }

    const std::string tmp_in  = file_in  + "._t1";
    const std::string tmp_out = file_out + "._t2";
    try {
        write_bytes(tmp_in, std::vector<uint8_t>(raw.begin() + static_cast<ptrdiff_t>(pos), raw.end()));
    } catch (const std::exception& e) { std::cerr << e.what() << "\n"; return false; }

    if (!decompress(tmp_in, tmp_out)) {
        std::remove(tmp_in.c_str());
        return false;
    }
    std::remove(tmp_in.c_str());

    std::vector<uint8_t> expanded;
    try { expanded = read_bytes(tmp_out); }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::remove(tmp_out.c_str());
        return false;
    }
    std::remove(tmp_out.c_str());

    std::vector<uint8_t> restored = undo_trans(expanded, opt, bwt_idx);
    try { write_bytes(file_out, restored); }
    catch (const std::exception& e) { std::cerr << e.what() << "\n"; return false; }
    return true;
}