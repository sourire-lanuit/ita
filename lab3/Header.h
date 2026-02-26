#ifndef HEADER_H
#define HEADER_H

#include <fstream>
#include <vector>
#include <cstdint>

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

#endif