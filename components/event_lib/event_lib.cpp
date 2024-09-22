#include <stdio.h>
#include "event_lib.h"

inline std::pair<uint8_t, uint8_t> compute_distance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){
    return std::make_pair(std::abs(x1 - x2), std::abs(y1 - y2));
}

bool is_noise(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int delta){
    std::pair<int, int> distance = compute_distance(x1, y1, x2, y2);
    return (distance.first > delta || distance.second > delta);
}

void collect_compressed_points(uint8_t temp_x, uint8_t temp_y, int count, std::vector<std::pair<uint8_t, uint8_t>> &compressed_data){
    uint8_t avg_x = temp_x/count;
    uint8_t avg_y = temp_y/count;
    compressed_data.emplace_back(avg_x, avg_y);
}