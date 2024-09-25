# include "compressor.hpp"

Compressor::Compressor(uint8_t delta_value, int margin): delta(delta_value), count_margin(margin) {}

bool Compressor::is_within_delta(const std::pair<uint8_t, uint8_t>& distance){
    return (distance.first <= delta && distance.second <= delta);
}

void Compressor::collect_compressed_points(uint16_t temp_x, uint16_t temp_y, int count, float& x_out, float& y_out){
    if (count > 0) {
            x_out = static_cast<float>(temp_x) / count;
            y_out = static_cast<float>(temp_y) / count;
        }
}

void Compressor::collect_compressed_points(uint16_t temp_x, uint16_t temp_y, int count, float& x_out, float& y_out){
    if (count > 0) {
            x_out = static_cast<float>(temp_x) / count;
            y_out = static_cast<float>(temp_y) / count;
        }
}
