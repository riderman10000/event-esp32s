#ifndef MY_DTW_HPP
#define MY_DTW_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>


// helper function to compute Euclidean distance between two points 
double euclideanDistance(double x, double y) ;
// helper function to create a cost matrix for DTW 
std::vector<std::vector<double>> createCostMatrix(int rows, int cols);
// Dynamic Time Warping algorithm 
double fastdtw(const std::vector<double>& series1, const std::vector<double>& series2);


#endif