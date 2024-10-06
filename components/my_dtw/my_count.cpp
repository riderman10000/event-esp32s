#include "my_count.hpp"

// C++ implementation of the get_count function
int get_count(const std::vector<double>& result) {
    int count = 1;
    for (size_t i = 1; i < result.size(); i++) {
        if (result[i] == 0 || result[i - 1] == 0) {
            continue;
        }
        if (std::abs(result[i] - result[i - 1]) <= 2) {
            count++;
        } else {
            return count;
        }
    }
    return count;
}

// Main function to compute dtw mean cost
int get_dtw_mean_cost(
    const std::vector<int>& win_change, 
    const std::vector<int>& top_win, 
    const std::vector<double>& data) {

    int win_change_length = win_change.size();
    int top_count = std::count(top_win.begin(), top_win.end(), 1);
    int bottom_count = win_change_length - top_count;

    std::vector<std::vector<double>> avg_cost_by_dtw_top(top_count, std::vector<double>(top_count, 0));
    std::vector<std::vector<double>> avg_cost_by_dtw_bottom(bottom_count, std::vector<double>(bottom_count, 0));

    int k_top = 0, k_bottom = 0;
    int g_top = 0, g_bottom = 0;

    for (int i = 0; i < win_change_length - 1; i++) {
        if (top_win[i] == top_win[i + 1]) continue;

        if (top_win[i] == 1) {
            g_top = k_top + 1;
        } else {
            g_bottom = k_bottom + 1;
        }

        for (int j = i + 1; j < win_change_length - 1; j++) {
            if (top_win[j] == top_win[j + 1]) continue;

            if (top_win[i] == 1 && top_win[j] == 1) {
                double cost = fastdtw(
                    std::vector<double>(data.begin() + win_change[i], data.begin() + win_change[i + 1]),
                    std::vector<double>(data.begin() + win_change[j], data.begin() + win_change[j + 1])
                );

                int num = (win_change[j + 1] - win_change[j]) + (win_change[i + 1] - win_change[i]);
                avg_cost_by_dtw_top[k_top][g_top] = cost / num;
                avg_cost_by_dtw_top[g_top][k_top] = cost / num;
                g_top++;
            } else if (top_win[i] == 0 && top_win[j] == 0) {
                double cost = fastdtw(
                    std::vector<double>(data.begin() + win_change[i], data.begin() + win_change[i + 1]),
                    std::vector<double>(data.begin() + win_change[j], data.begin() + win_change[j + 1])
                );

                int num = (win_change[j + 1] - win_change[j]) + (win_change[i + 1] - win_change[i]);
                avg_cost_by_dtw_bottom[k_bottom][g_bottom] = cost / num;
                avg_cost_by_dtw_bottom[g_bottom][k_bottom] = cost / num;
                g_bottom++;
            }
        }

        if (top_win[i] == 1) {
            k_top++;
        } else {
            k_bottom++;
        }
    }

    // Remove the last row/column (like slicing in Python)
    avg_cost_by_dtw_top.resize(avg_cost_by_dtw_top.size() - 1);
    avg_cost_by_dtw_bottom.resize(avg_cost_by_dtw_bottom.size() - 1);

    for (size_t i = 0; i < avg_cost_by_dtw_top.size(); i++) {
        if (avg_cost_by_dtw_top[i].size() > 1) {
            double temp = std::accumulate(avg_cost_by_dtw_top[i].begin(), avg_cost_by_dtw_top[i].end(), 0.0) / (avg_cost_by_dtw_top[i].size() - 1);
            std::replace(avg_cost_by_dtw_top[i].begin(), avg_cost_by_dtw_top[i].end(), 0.0, temp);
        }
    }

    for (size_t i = 0; i < avg_cost_by_dtw_bottom.size(); i++) {
        if (avg_cost_by_dtw_bottom[i].size() > 1) {
            double temp = std::accumulate(avg_cost_by_dtw_bottom[i].begin(), avg_cost_by_dtw_bottom[i].end(), 0.0) / (avg_cost_by_dtw_bottom[i].size() - 1);
            std::replace(avg_cost_by_dtw_bottom[i].begin(), avg_cost_by_dtw_bottom[i].end(), 0.0, temp);
        }
    }

    std::vector<double> result_top;
    std::vector<double> result_bottom;

    if (!avg_cost_by_dtw_top.empty()) {
        for (size_t i = 0; i < avg_cost_by_dtw_top.size(); i++) {
            result_top.push_back(std::accumulate(avg_cost_by_dtw_top[i].begin(), avg_cost_by_dtw_top[i].end(), 0.0) / avg_cost_by_dtw_top[i].size());
        }
        std::sort(result_top.begin(), result_top.end());
    }

    if (!avg_cost_by_dtw_bottom.empty()) {
        for (size_t i = 0; i < avg_cost_by_dtw_bottom.size(); i++) {
            result_bottom.push_back(std::accumulate(avg_cost_by_dtw_bottom[i].begin(), avg_cost_by_dtw_bottom[i].end(), 0.0) / avg_cost_by_dtw_bottom[i].size());
        }
        std::sort(result_bottom.begin(), result_bottom.end());
    }

    int count_top = get_count(result_top);
    int count_bottom = get_count(result_bottom);

    int temp = count_top > count_bottom ? count_top : count_bottom;
    printf("COUNT IS : %d", temp);

    return count_top > count_bottom ? count_top : count_bottom;
}
