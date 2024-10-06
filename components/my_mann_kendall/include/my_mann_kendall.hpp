# ifndef MY_MANN_KENDALL_HPP 
# define MY_MANN_KENDALL_HPP

#include "esp_mac.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

// mann-kendall test implementation
int mann_kendall_test(const std::vector<double>& data);
// variance of the difference 
double variance(const std::vector<int>& changes);
// function to find monotonous jump point 
void get_index_of_bottom_and_top_by_mk(const std::vector<double>& data);

#endif 