#include "my_dtw.hpp"

using namespace std;

// Structure to hold the result of DTW
struct DTWResult {
    double distance;
    std::vector<std::pair<int, int>> path;
};

// Euclidean distance between two points
double euclideanDistance(double x, double y) {
    return pow(x - y, 2);
}

// Base case DTW (dynamic time warping) on two time series
DTWResult dtw(const std::vector<double>& x, const std::vector<double>& y) {
    int n = x.size();
    int m = y.size();
    std::vector<std::vector<double>> dtw_matrix(n + 1, std::vector<double>(m + 1, std::numeric_limits<double>::infinity()));
    
    dtw_matrix[0][0] = 0;
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            double cost = euclideanDistance(x[i - 1], y[j - 1]);
            dtw_matrix[i][j] = cost + min({dtw_matrix[i - 1][j], dtw_matrix[i][j - 1], dtw_matrix[i - 1][j - 1]});
        }
    }
    
    // Backtrack to find the optimal path
    std::vector<std::pair<int, int>> path;
    int i = n, j = m;
    while (i > 0 && j > 0) {
        path.emplace_back(i - 1, j - 1);
        if (i == 1) --j;
        else if (j == 1) --i;
        else {
            double min_val = min({dtw_matrix[i - 1][j], dtw_matrix[i][j - 1], dtw_matrix[i - 1][j - 1]});
            if (min_val == dtw_matrix[i - 1][j - 1]) {
                --i;
                --j;
            } else if (min_val == dtw_matrix[i - 1][j]) {
                --i;
            } else {
                --j;
            }
        }
    }
    reverse(path.begin(), path.end());
    
    return {dtw_matrix[n][m], path};
}

// Downsample the time series by reducing its size by half
std::vector<double> downsample(const std::vector<double>& series) {
    std::vector<double> downsampled;
    for (int i = 0; i < series.size(); i += 2) {
        downsampled.push_back(series[i]);
    }
    return downsampled;
}

// FastDTW implementation
DTWResult fastDTW(const std::vector<double>& x, const std::vector<double>& y, int radius) {
    if (x.size() <= radius + 1 || y.size() <= radius + 1) {
        return dtw(x, y); // Use classic DTW for small sequences
    }
    
    // Recursive call with downsampled sequences
    std::vector<double> downsampled_x = downsample(x);
    std::vector<double> downsampled_y = downsample(y);
    
    DTWResult downsampled_dtw = fastDTW(downsampled_x, downsampled_y, radius);
    
    // Now upsample the result (expand the path to match the original size)
    std::vector<std::pair<int, int>> window;
    for (const auto& p : downsampled_dtw.path) {
        int start_x = max(0, 2 * p.first - radius);
        int end_x = min(static_cast<int>(x.size()), 2 * p.first + radius);
        int start_y = max(0, 2 * p.second - radius);
        int end_y = min(static_cast<int>(y.size()), 2 * p.second + radius);
        for (int i = start_x; i < end_x; ++i) {
            for (int j = start_y; j < end_y; ++j) {
                window.emplace_back(i, j);
            }
        }
    }
    
    // Classic DTW on the small window
    std::vector<std::vector<double>> dtw_matrix(x.size() + 1, std::vector<double>(y.size() + 1, std::numeric_limits<double>::infinity()));
    dtw_matrix[0][0] = 0;
    
    for (const auto& w : window) {
        int i = w.first + 1;
        int j = w.second + 1;
        double cost = euclideanDistance(x[i - 1], y[j - 1]);
        dtw_matrix[i][j] = cost + min({dtw_matrix[i - 1][j], dtw_matrix[i][j - 1], dtw_matrix[i - 1][j - 1]});
    }
    
    // Backtrack to find the optimal path
    std::vector<std::pair<int, int>> path;
    int i = x.size(), j = y.size();
    while (i > 0 && j > 0) {
        path.emplace_back(i - 1, j - 1);
        if (i == 1) --j;
        else if (j == 1) --i;
        else {
            double min_val = min({dtw_matrix[i - 1][j], dtw_matrix[i][j - 1], dtw_matrix[i - 1][j - 1]});
            if (min_val == dtw_matrix[i - 1][j - 1]) {
                --i;
                --j;
            } else if (min_val == dtw_matrix[i - 1][j]) {
                --i;
            } else {
                --j;
            }
        }
    }
    reverse(path.begin(), path.end());
    
    return {dtw_matrix[x.size()][y.size()], path};
}

// int main() {
//     // Example time series
//     vector<double> x = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//     vector<double> y = {1, 3, 4, 9, 8, 10};
    
//     // Radius for FastDTW
//     int radius = 1;
    
//     // Perform FastDTW
//     DTWResult result = fastDTW(x, y, radius);
    
//     // Output the result
//     cout << "FastDTW Distance: " << result.distance << endl;
//     cout << "Path: ";
//     for (const auto& p : result.path) {
//         cout << "(" << p.first << "," << p.second << ") ";
//     }
//     cout << endl;
    
//     return 0;
// }
