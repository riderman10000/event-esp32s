#ifndef MY_COUNT_HPP
#define MY_COUNT_HPP


#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm> // For std::sort
#include <numeric>   // For std::accumulate

#include "my_dtw.hpp"

// C++ implementation of the get_count function
int get_count(const std::vector<double>& result);
// Main function to compute dtw mean cost
int get_dtw_mean_cost(
    const std::vector<int>& win_change, 
    const std::vector<int>& top_win, 
    const std::vector<double>& data);
#endif 