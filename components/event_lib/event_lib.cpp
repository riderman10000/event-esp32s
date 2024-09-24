#include <stdio.h>
#include "event_lib.hpp"

const char * EVENT_LIB_TAG = "EVENT LIB TAG";

inline std::pair<uint8_t, uint8_t> compute_distance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){
    return std::make_pair(std::abs(x1 - x2), std::abs(y1 - y2));
}

bool is_noise(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int delta){
    std::pair<int, int> distance = compute_distance(x1, y1, x2, y2);
    return (distance.first > delta || distance.second > delta);
}

void collect_compressed_points(uint16_t temp_x, uint16_t temp_y, int count, float &x, float &y){
    ESP_LOGI(EVENT_LIB_TAG, "Collect Compressed points");
    x = static_cast<float>(temp_x)/count;
    y = static_cast<float>(temp_y)/count;
    ESP_LOGI(EVENT_LIB_TAG, "Collect Compressed points %f, %f \n", x, y);
}