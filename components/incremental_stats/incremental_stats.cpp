#include <stdio.h>
#include "incremental_stats.hpp"

IncrementalStatistics::IncrementalStatistics() : count(0), total_sum(cv::Mat::zeros(1, 1, CV_64F)), total_squared_sum(cv::Mat::zeros(1, 1, CV_64F)) {}

// Add a chunk of data points
void IncrementalStatistics::addDataChunk(const cv::Mat& chunk) {
    cv::Scalar chunk_mean, chunk_stddev;

    // Calculate mean and standard deviation for the current chunk
    cv::meanStdDev(chunk, chunk_mean, chunk_stddev);
    // std::cout << "mean " << chunk_mean << std::endl;
    // std::cout << "std " << chunk_stddev << std::endl;

    // Update the running totals for mean and squared mean (for variance calculation)
    double chunk_size = static_cast<double>(chunk.rows * chunk.cols);
    total_sum.at<double>(0, 0) += chunk_mean[0] * chunk_size;
    total_squared_sum.at<double>(0, 0) += (chunk_mean[0] * chunk_mean[0] + chunk_stddev[0] * chunk_stddev[0]) * chunk_size;
    count += chunk_size;
}

// Get the mean
double IncrementalStatistics::getMean() const {
    return count > 0 ? total_sum.at<double>(0, 0) / count : 0.0;
}

// Get the variance (unbiased)
double IncrementalStatistics::getVariance() const {
    if (count <= 1) return 0.0;
    double mean = getMean();
    return (total_squared_sum.at<double>(0, 0) / count) - (mean * mean);
}

// Get the standard deviation
double IncrementalStatistics::getStandardDeviation() const {
    return std::sqrt(getVariance());
}