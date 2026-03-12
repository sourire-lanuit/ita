#include "Header.h"
#include <cassert>

static uint32_t bits_to_encode(uint32_t n) {
    if (n == 0) return 1;
    uint32_t b = 0, v = n;
    while (v) { v >>= 1; ++b; }
    return b;
}

static uint32_t calc_bits(uint32_t next, uint32_t max_b) {
    uint32_t top = (next > 0) ? (next - 1) : 0;
    uint32_t b   = bits_to_encode(top);
    if (b < 9)     b = 9;
    if (b > max_b) b = max_b;
    return b;
}

CompressDict::CompressDict(uint32_t max_code_bits) : max_b(max_code_bits) {
    if (max_code_bits < 9 || max_code_bits > 16) throw std::invalid_argument("max_bits 9-16");
    max = 1u << max_code_bits;
    reset();
}

void CompressDict::reset() {
    nodes.clear();
    nodes.emplace_back(); 
    for (uint32_t c = 0; c < alph_size; ++c) {
        uint32_t idx = (uint32_t)nodes.size();
        nodes.emplace_back();
        nodes[idx].code = c;
        nodes[0].nashch[c] = (int32_t)idx;
    }

    next = alph_size;
    cursor = 0;
    curr = UINT32_MAX;
}

bool CompressDict::step(uint8_t b) {
    int32_t nash = nodes[cursor].nashch[b];
    if (nash == -1) return false;
    cursor = nash;
    curr = nodes[cursor].code;
    return true;
}

void CompressDict::add_phrase(uint8_t b) {
    if (!is_full()) {
        uint32_t new_idx = (uint32_t)nodes.size();
        nodes.emplace_back();
        nodes[new_idx].code = next++;
        nodes[cursor].nashch[b] = (int32_t)new_idx;
    }
}

void CompressDict::restart(uint8_t b) {
    cursor = 0;
    int32_t nash = nodes[0].nashch[b];
    assert(nash != -1);
    cursor = nash;
    curr = nodes[cursor].code;
}

uint32_t CompressDict::current_bits() const {
    return calc_bits(next, max_b);
}

DecompressDict::DecompressDict(uint32_t max_code_bits) : max_b(max_code_bits) {
    if (max_code_bits < 9 || max_code_bits > 16) throw std::invalid_argument("max_bits 9-16");
    max = 1u << max_code_bits;
    reset();
}

void DecompressDict::reset() {
    table.clear();
    table.resize(alph_size);
    for (uint32_t c = 0; c < alph_size; ++c) table[c] = { static_cast<uint8_t>(c) };
    next = alph_size;
}

const std::vector<uint8_t>& DecompressDict::decode(uint32_t code) const {
    if (code >= table.size() || table[code].empty()) throw std::runtime_error("декомпресія не спрацювала " + std::to_string(code));
    return table[code];
}

void DecompressDict::add_entry(uint32_t prev_code, uint32_t cur_code) {
    if (is_full()) return;
    const std::vector<uint8_t>& prev = decode(prev_code);
    std::vector<uint8_t> entry = prev;
    if (cur_code < (uint32_t)table.size() && !table[cur_code].empty()) entry.push_back(table[cur_code][0]);
    else entry.push_back(prev[0]);
    table.push_back(std::move(entry));
    ++next;
}

uint32_t DecompressDict::current_bits() const {
    return calc_bits(next + 1, max_b);
}