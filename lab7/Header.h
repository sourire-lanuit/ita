#ifndef HEADER_H
#define HEADER_H

#include <vector>
#include <string>
#include <random>

static constexpr uint32_t N = 1u << 16; 
static constexpr uint32_t mod = N;

uint32_t hpoly(uint32_t k, const std::string& s);

class Bloom_Filter {
public:
    explicit Bloom_Filter(int s_count, std::mt19937& rng);
    void add(const std::string& x);
    bool contains(const std::string& x) const; 

    void clear() {
        std::fill(bits.begin(), bits.end(), 0);
    }

private:
    int s;
    std::vector<uint64_t> bits; 
    std::vector<uint32_t> keys;

    void set_bit(uint32_t idx) {
        bits[idx / 64] |= (1ULL << (idx % 64));
    }

    bool get_bit(uint32_t idx) const {
        return (bits[idx / 64] >> (idx % 64)) & 1ULL;
    }
};

std::string random_string(std::mt19937& rng); 
double one_experiment(double alpha, int s_val, std::mt19937& rng);

#endif