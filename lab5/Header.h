#ifndef HEADER_H
#define HEADER_H

#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>

struct BSt_Writer {
    std::ofstream file;
    uint8_t buffer = 0;
    int pos = 0; 
};

struct BSt_Reader {
    std::ifstream file;
    uint8_t buffer = 0;
    int pos = 8; 
};

bool o_writer(BSt_Writer& bs, const std::string& file_name);
void x_writer(BSt_Writer& bs);
void write_bs(BSt_Writer& bs, const std::vector<uint8_t>& data, int count);
bool o_reader(BSt_Reader& bs, const std::string& file_name);
void x_reader(BSt_Reader& bs);
void read_bs(BSt_Reader& bs, std::vector<uint8_t>& data, int count);

static constexpr uint32_t alph_size = 256;

struct Compress {
    int32_t nashch[alph_size];
    uint32_t code;
    Compress() : code(0) {
        std::fill(std::begin(nashch), std::end(nashch), -1);
    }
};

class CompressDict {
public:
    explicit CompressDict(uint32_t max_code_bits);
 
    void reset();
    bool step(uint8_t b);        
    void add_phrase(uint8_t b); 
    void restart(uint8_t b);
    uint32_t current_code() const { return curr; }
    bool is_full() const { return next >= max; }
    uint32_t current_bits() const;
 
private:
    std::vector<Compress> nodes;
    uint32_t next;
    uint32_t max;
    uint32_t max_b;
    int32_t cursor;
    uint32_t curr;
};
 
class DecompressDict {
public:
    explicit DecompressDict(uint32_t max_code_bits);
 
    void reset();
    const std::vector<uint8_t>& decode(uint32_t code) const;
    void add_entry(uint32_t prev_code, uint32_t cur_code);
    bool is_full() const { return next >= max; }
    uint32_t next_code() const { return next; }
    uint32_t current_bits() const;
 
private:
    std::vector<std::vector<uint8_t>> table;
    uint32_t next;
    uint32_t max;
    uint32_t max_b;
};

static constexpr uint8_t lzw[4] = {'L', 'Z', 'W', '\x01'};
static constexpr uint32_t lzw_size = 10;
static constexpr uint32_t lzw_min = 9;
static constexpr uint32_t lzw_max = 16;
static constexpr uint32_t lzw_def = 12; 

void lzw_compr(const std::string& src_file, const std::string& dst_file, uint32_t max_bits = lzw_def, bool reset = false);
void lzw_decompr(const std::string& src_file, const std::string& dst_file);

#endif