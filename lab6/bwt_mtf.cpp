#include "Header.h"

BWT bwt_encode(const std::vector<uint8_t>& in) {
    if (in.empty()) return {{}, 0};
    const size_t n = in.size();
    std::vector<int> rnk(n), tmp(n);
    std::vector<size_t> sa(n);
    std::iota(sa.begin(), sa.end(), 0);
    for (size_t i = 0; i < n; ++i) rnk[i] = in[i];
    for (size_t gap = 1; gap < n; gap <<= 1) {
        auto cmp = [&](size_t a, size_t b) {
            if (rnk[a] != rnk[b]) return rnk[a] < rnk[b];
            return rnk[(a + gap) % n] < rnk[(b + gap) % n];
        };
        std::sort(sa.begin(), sa.end(), cmp);
        tmp[sa[0]] = 0;
        for (size_t i = 1; i < n; ++i) tmp[sa[i]] = tmp[sa[i-1]] + (cmp(sa[i-1], sa[i]) ? 1 : 0);
        rnk = tmp;
        if (rnk[sa[n-1]] == static_cast<int>(n-1)) break;
    }

    BWT r;
    r.data.resize(n);
    r.primary_index = 0;
    for (size_t i = 0; i < n; ++i) {
        r.data[i] = in[(sa[i] + n - 1) % n];
        if (sa[i] == 0) r.primary_index = static_cast<uint32_t>(i);
    }
    return r;
}

std::vector<uint8_t> bwt_decode(const std::vector<uint8_t>& bwt_data, uint32_t primary_index) {
    if (bwt_data.empty()) return {};
    const size_t n = bwt_data.size();
    size_t cnt[256]  = {};
    for (uint8_t b : bwt_data) cnt[b]++;
    size_t start[256] = {};
    for (int c = 1; c < 256; ++c) start[c] = start[c-1] + cnt[c-1];
    std::vector<size_t> rank_arr(n);
    size_t cnt2[256] = {};
    for (size_t i = 0; i < n; ++i) rank_arr[i] = cnt2[bwt_data[i]]++;
    std::vector<uint8_t> result(n);
    size_t pos = primary_index;
    for (size_t i = n; i-- > 0; ) {
        result[i] = bwt_data[pos];
        pos = start[bwt_data[pos]] + rank_arr[pos];
    }
    return result;
}

std::vector<uint8_t> mtf_encode(const std::vector<uint8_t>& in) {
    uint8_t alphabet[256];
    for (int i = 0; i < 256; ++i) alphabet[i] = static_cast<uint8_t>(i);
    std::vector<uint8_t> out;
    out.reserve(in.size());
    for (uint8_t byte : in) {
        int pos = 0;
        while (alphabet[pos] != byte) ++pos;
        out.push_back(static_cast<uint8_t>(pos));
        for (int i = pos; i > 0; --i) alphabet[i] = alphabet[i-1];
        alphabet[0] = byte;
    }
    return out;
}

std::vector<uint8_t> mtf_decode(const std::vector<uint8_t>& in) {
    uint8_t alphabet[256];
    for (int i = 0; i < 256; ++i) alphabet[i] = static_cast<uint8_t>(i);
    std::vector<uint8_t> out;
    out.reserve(in.size());
    for (uint8_t idx : in) {
        uint8_t byte = alphabet[idx];
        out.push_back(byte);
        for (int i = idx; i > 0; --i) alphabet[i] = alphabet[i-1];
        alphabet[0] = byte;
    }
    return out;
}

std::vector<uint8_t> read_bytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Файл не відкриваєцця");
    return { std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>() };
}

void write_bytes(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Файл не записуєцця");
    f.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

std::vector<uint8_t> apply_trans(const std::vector<uint8_t>& data, const trans_opt& opt, uint32_t& bwt_idx) {
    std::vector<uint8_t> result = data;
    bwt_idx = 0;
    if (opt.use_bwt) {
        BWT r = bwt_encode(result);
        result  = std::move(r.data);
        bwt_idx = r.primary_index;
    }
    if (opt.use_mtf) result = mtf_encode(result);
    return result;
}

std::vector<uint8_t> undo_trans(const std::vector<uint8_t>& data, const trans_opt& opt, uint32_t bwt_idx) {
    std::vector<uint8_t> result = data;
    if (opt.use_mtf) result = mtf_decode(result);
    if (opt.use_bwt) result = bwt_decode(result, bwt_idx);
    return result;
}