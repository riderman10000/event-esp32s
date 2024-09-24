#ifndef COMPRESSOR_H
#define COMPRESSOR_H 

#include <stdio.h>
#include <utility>

class Compressor{
private:
    uint8_t delta;
    int count_margin;

public:
    Compressor(uint8_t delta_value, int margin) ; //: delta(delat_value), count_margin(margin) {}
    bool is_within_delta(const std::pair<uint8_t, uint8_t>& distance);
    void collect_compressed_points(uint16_t temp_x, uint16_t temp_y, int count, float& x_out, float& y_out);

};
#endif 