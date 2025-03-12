#ifndef CSV_DATASET_H
#define CSV_DATASET_H

#include <vector>
#include <cstdlib>
#include <iostream>

struct Point {
    std::vector<double> coords;
    int cluster;
};

struct Centroid {
    std::vector<double> coords;
};


std::vector<Point> loadCSV(const std::string& filename, int *dimensions, const std::vector<int>& excludeCols);

#endif

