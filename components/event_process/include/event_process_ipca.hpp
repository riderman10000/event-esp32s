# ifndef EVENT_PROCESS_IPCA_H
# define EVENT_PROCESS_IPCA_H

#include <opencv2/opencv.hpp>
#include <iostream>

class IncrementalPCA {
private:
    int n_samples_seen = 0;
    int n_components = 1;
    cv::Mat mean;
    cv::Mat cov_matrix;
public:
    IncrementalPCA(int components);

    // Incrementally update the covariance matrix
    void partial_fit(const cv::Mat& chunk_data);
    // Transform the data using the principal components
    cv::Mat transform(const cv::Mat& data);
};

// int main() {
//     // Simulate some chunked data
//     cv::Mat data_chunk1 = (cv::Mat_<double>(4, 3) << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
//     cv::Mat data_chunk2 = (cv::Mat_<double>(4, 3) << 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

//     IncrementalPCA ipca(2);  // Create an IPCA with 2 components

//     // Process the first chunk of data
//     ipca.partial_fit(data_chunk1);

//     // Process the second chunk of data
//     ipca.partial_fit(data_chunk2);

//     // Now transform the data using the principal components
//     cv::Mat transformed_data = ipca.transform(data_chunk1);

//     std::cout << "Transformed data:\n" << transformed_data << std::endl;

//     return 0;
// }

# endif