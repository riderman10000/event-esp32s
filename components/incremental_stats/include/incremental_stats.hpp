#ifndef INCREMENTAL_STATS_HPP
#define INCREMENTAL_STATS_HPP


#include <iostream>
#include <opencv2/opencv.hpp>  // Include OpenCV library

// Class to handle incremental calculation of mean and standard deviation using OpenCV
class IncrementalStatistics {
public:
    IncrementalStatistics(); 

    // Add a chunk of data points
    void addDataChunk(const cv::Mat& chunk);

    // Get the mean
    double getMean() const ;

    // Get the variance (unbiased)
    double getVariance() const ;

    // Get the standard deviation
    double getStandardDeviation() const ;

    

private:
    int count;                      // Total number of data points
    cv::Mat total_sum;               // Sum of all elements for mean calculation
    cv::Mat total_squared_sum;       // Sum of squares for variance calculation
};

#endif 