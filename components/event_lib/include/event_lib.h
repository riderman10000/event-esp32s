#ifndef EVENT_LIB_H
#define EVENT_LIB_H

#include <stdio.h>
#include <sstream>
#include <string.h>
#include "vector"

inline std::pair<uint8_t, uint8_t> compute_distance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
bool is_noise(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int delta);
void collect_compressed_points(uint8_t temp_x, uint8_t temp_y, int count, std::vector<std::pair<uint8_t, uint8_t>> &compressed_data);

#endif 