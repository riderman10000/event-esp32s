# ifndef EVENT_PROCESS_IPCA_H
# define EVENT_PROCESS_IPCA_H


#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class IncrementalPCA {
private:
    int n_components;
    Mat mean;
    Mat cov_matrix;
    int n_samples_seen;

public:
    // Constructor
    IncrementalPCA(int components) : n_components(components), n_samples_seen(0) {}
    Mat transform(const Mat& data);
    void partial_fit(const Mat& chunk_data);
};

# endif