#include "Header.h"
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <cassert>

uint32_t hpoly(uint32_t k, const std::string& s) {
    std::string padded = s;
    if (padded.size() % 2 != 0) padded.push_back('\0');

    uint32_t Y = 1;
    for (size_t i = 0; i < padded.size(); i += 2) {
        uint32_t xi = static_cast<uint8_t>(padded[i]) + static_cast<uint8_t>(padded[i + 1]) * 256u;
        Y = (Y * k + xi) % mod;
    }
    return Y;
}

Bloom_Filter::Bloom_Filter(int s_count, std::mt19937& rng) : s(s_count), bits(N / 64, 0) {
        std::uniform_int_distribution<uint32_t> dist(1, mod - 1);
        keys.reserve(s);
        while (static_cast<int>(keys.size()) < s) {
            uint32_t k = dist(rng);
            bool dup = false;
            for (auto kk : keys) if (kk == k) { dup = true; break; }
            if (!dup) keys.push_back(k);
        }
}

void Bloom_Filter::add(const std::string& x) {
        for (int i = 0; i < s; ++i) {
            uint32_t idx = hpoly(keys[i], x);
            set_bit(idx);
        }
}

bool Bloom_Filter::contains(const std::string& x) const {
        for (int i = 0; i < s; ++i) {
            uint32_t idx = hpoly(keys[i], x);
            if (!get_bit(idx)) return false;
        }
        return true;
}

std::string random_string(std::mt19937& rng) {
    static std::uniform_int_distribution<int> len_dist(1, 100);
    static std::uniform_int_distribution<int> chr_dist(32, 126); 
    int len = len_dist(rng);
    std::string s(len, ' ');
    for (auto& c : s) c = static_cast<char>(chr_dist(rng));
    return s;
}

double one_experiment(double alpha, int s_val, std::mt19937& rng) {
    Bloom_Filter bf(s_val, rng);
    int n = static_cast<int>(std::round(alpha * N));

    std::vector<std::string> added(n);
    for (int i = 0; i < n; ++i) {
        added[i] = random_string(rng);
        bf.add(added[i]);
    }

    long long M = 0;
    while (true) {
        ++M;
        std::string candidate = random_string(rng);
        if (bf.contains(candidate)) {
           break;
        }
        if (M > 10'000'000) break;
    }
    return 1.0 / static_cast<double>(M);
}