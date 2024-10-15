#include <stdio.h>
#include "my_mann_kendall.hpp"

#include "esp_mac.h"
#include "my_count.hpp"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Mann-Kendall Test Implementation
// Return 1 for increasing trend, -1 for decreasing trend, 0 for no trend
int mann_kendall_test(const std::vector<double>& data) {
    int n = data.size();
    int S = 0;

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (data[j] > data[i]) {
                S += 1;
            } else if (data[j] < data[i]) {
                S -= 1;
            }
        }
    }

    // Trend determination
    if (S > 0) return 1;  // increasing
    else if (S < 0) return -1;  // decreasing
    else return 0;  // no trend
}

// Variance of the differences
double variance(const std::vector<int>& changes) {
    std::vector<int> diff;
    for (int i = 1; i < changes.size(); ++i) {
        diff.push_back(changes[i] - changes[i - 1]);
    }

    double mean_diff = std::accumulate(diff.begin(), diff.end(), 0.0) / diff.size();
    double var_sum = 0.0;
    
    for (size_t i = 0; i < diff.size(); ++i) {
        var_sum += (diff[i] - mean_diff) * (diff[i] - mean_diff);
    }
    
    return var_sum / diff.size();
}

// Main function to find monotonous jump points
// std::tuple<std::vector<int>, std::vector<int>, int, int> get_index_of_bottom_and_top_by_mk(const std::vector<double>& data) {
void get_index_of_bottom_and_top_by_mk(const std::vector<double>& data) {
    int win_size = 10;
    int step = 5;

    double first = std::numeric_limits<double>::infinity();
    double second = -std::numeric_limits<double>::infinity();
    bool flag = true;
    std::vector<int> final_win_change;

    while (second < first) {
        // record the total number of pane divided.
        int win_sum = (data.size() > win_size) ? 1 + (data.size() - win_size) / step : 0;
        // each pane starting index 
        std::vector<int> win_start_index(win_sum);

        // Fill starting indices of each window
        for (int i = 0; i < win_sum; ++i) {
            win_start_index[i] = i * step;
        }
        // mutation pane index [later processing as an intermediate index ]
        std::vector<int> win_change;
        // current window tred [increasing, decreasing, no trend]
        std::vector<int> win_trend;

        // Traverse each window and perform Mann-Kendall test
        for (int i = 0; i < win_sum; ++i) {
            std::vector<double> win_data(data.begin() + win_start_index[i], data.begin() + win_start_index[i] + win_size);
            win_trend.push_back(mann_kendall_test(win_data));  // Store trend result (1, -1, or 0)
        }

        // Detect change points (mutations)
        for (int i = 1; i < win_trend.size(); ++i) {
            if (win_trend[i] == 1 && win_trend[i - 1] != 1) {
                win_change.push_back(win_start_index[i - 1]);
            } else if (win_trend[i] == -1 && win_trend[i - 1] != -1) {
                win_change.push_back(win_start_index[i - 1]);
            }
        }

        // Adjust the window change points
        for (int i = 0; i < win_change.size(); ++i) {
            win_change[i] += win_size / 2;
        }

        // Add boundaries
        win_change.insert(win_change.begin(), 0);
        win_change.push_back(data.size() - 1);

        // Compute variance of the differences in window changes
        double var = variance(win_change);

        if (flag) {
            first = var;
        } else {
            second = var;
        }

        // Break condition if second variance exceeds first
        if (second >= first) {
            break;
        }

        flag = !flag;
        win_size += 5;
        final_win_change = win_change;
    }

    // Determine rising or falling edges
    std::vector<int> top_win(final_win_change.size(), 0);
    if (data[final_win_change[0]] <= data[final_win_change[1]]) {
        top_win[0] = 1;
    }
    for (int i = 1; i < final_win_change.size(); ++i) {
        if (data[final_win_change[i - 1]] >= data[final_win_change[i]]) {
            top_win[i] = 1;
        }
    }

    // print the top_win and win_change_data...
    // for(int i = 0; i < final_win_change.size(); i++){
    //     printf("%d,%d,%d\n", i, final_win_change[i], top_win[i]);
    //     vTaskDelay(5);
        
    // }

    get_dtw_mean_cost(final_win_change, top_win, data);

    // for(int i = 0; i < top_win.size(); i++){
    //     printf("top win change %d -- %d \n", i, top_win[i]);
    // }

    // return std::make_tuple(final_win_change, top_win, win_size, step);
}

// int main() {
//     std::vector<double> data = {1, 3, 5, 7, 9, 8, 6, 4, 2, 1, 3, 5, 7, 8, 9};

//     std::vector<int> final_win_change;
//     std::vector<int> top_win;
//     int win_size, step;

//     std::tie(final_win_change, top_win, win_size, step) = get_index_of_bottom_and_top_by_mk(data);

//     std::cout << "Final Window Change Indices: ";
//     for (size_t i = 0; i < final_win_change.size(); ++i) {
//         std::cout << final_win_change[i] << " ";
//     }
//     std::cout << "\nTop Win Indices: ";
//     for (size_t i = 0; i < top_win.size(); ++i) {
//         std::cout << top_win[i] << " ";
//     }
//     std::cout << "\nWindow Size: " << win_size;
//     std::cout << "\nStep Size: " << step << std::endl;

//     return 0;
// }
