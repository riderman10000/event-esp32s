#include "my_dtw.hpp"


// Helper function to compute Euclidean distance between two points
double euclideanDistance(double x, double y) {
    return std::sqrt((x - y) * (x - y));
}

// Helper function to create a cost matrix for DTW
std::vector<std::vector<double>> createCostMatrix(int rows, int cols) {
    std::vector<std::vector<double>> costMatrix(rows, std::vector<double>(cols, std::numeric_limits<double>::infinity()));
    return costMatrix;
}

// Dynamic Time Warping algorithm
double fastdtw(const std::vector<double>& series1, const std::vector<double>& series2) {
    int len1 = series1.size();
    int len2 = series2.size();

    // Create the cost matrix
    std::vector<std::vector<double>> costMatrix = createCostMatrix(len1, len2);

    // Initialize the first cell
    costMatrix[0][0] = euclideanDistance(series1[0], series2[0]);

    // Fill the cost matrix
    for (int i = 1; i < len1; i++) {
        costMatrix[i][0] = costMatrix[i - 1][0] + euclideanDistance(series1[i], series2[0]);
    }

    for (int j = 1; j < len2; j++) {
        costMatrix[0][j] = costMatrix[0][j - 1] + euclideanDistance(series1[0], series2[j]);
    }

    for (int i = 1; i < len1; i++) {
        for (int j = 1; j < len2; j++) {
            double cost = euclideanDistance(series1[i], series2[j]);
            costMatrix[i][j] = cost + std::min({costMatrix[i - 1][j], costMatrix[i][j - 1], costMatrix[i - 1][j - 1]});
        }
    }

    // The final DTW distance is the value in the bottom-right cell
    return costMatrix[len1 - 1][len2 - 1];
}

// int main() {
//     // Example time series data
//     std::vector<double> series1 = {1.0, 2.0, 3.0, 4.0, 5.0};
//     std::vector<double> series2 = {2.0, 3.0, 4.0, 5.0, 6.0};

//     // Perform DTW
//     double dtwDistance = dtw(series1, series2);

//     // Output the result
//     std::cout << "DTW Distance: " << dtwDistance << std::endl;

//     return 0;
// }
