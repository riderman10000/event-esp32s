#include "event_process_ipca.hpp"

// Function to incrementally update the covariance matrix
void IncrementalPCA::partial_fit(const Mat& chunk_data) {
    int n_samples = chunk_data.rows;
    int n_features = chunk_data.cols;

    // Initialize the mean and covariance matrix if it's the first batch
    if (n_samples_seen == 0) {
        mean = Mat::zeros(1, n_features, CV_32F);
        cov_matrix = Mat::zeros(n_features, n_features, CV_32F);
    }

    // Compute the mean for the current chunk
    Mat chunk_mean;
    reduce(chunk_data, chunk_mean, 0, REDUCE_AVG, CV_32F);

    // Center the chunk data
    Mat centered_chunk;
    subtract(chunk_data, chunk_mean, centered_chunk);

    // Update the covariance matrix
    cov_matrix += centered_chunk.t() * centered_chunk;

    // Update the running mean
    mean = (mean * n_samples_seen + chunk_mean * n_samples) / (n_samples_seen + n_samples);

    // Update the total number of samples processed
    n_samples_seen += n_samples;
}

// Transform the data using the principal components
Mat IncrementalPCA::transform(const Mat& data) {
    // Perform eigen decomposition of the covariance matrix
    Mat eigenvalues, eigenvectors;
    eigen(cov_matrix, eigenvalues, eigenvectors);

    // Select the top 'n_components' principal components
    Mat principal_components = eigenvectors.rowRange(eigenvectors.rows - n_components, eigenvectors.rows).clone();

    // Project the data onto the principal components
    Mat centered_data;
    subtract(data, mean, centered_data);
    Mat reduced_data = centered_data * principal_components.t();

    return reduced_data;
}