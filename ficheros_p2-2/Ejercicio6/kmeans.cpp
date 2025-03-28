#include <math.h>
#include <omp.h>

#include "csv_dataset.h"
#include "extra.h"


void initializeCentroids(std::vector<Centroid>& centroids, std::vector<Point>& data, int k) {
    srand(42);
    for (int i = 0; i < k; i++) {
        int randIdx = rand() % data.size();
        centroids[i].coords = data[randIdx].coords;
    }
}


// Compute Euclidean distance for N-dimensional space
double euclideanDistance(const Point& a, const Centroid& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.coords.size(); i++) {
        sum += pow(a.coords[i] - b.coords[i], 2);
    }
    return sqrt(sum);
}

// Assign clusters to points
void assignClusters(std::vector<Point>& data, std::vector<Centroid>& centroids, int k) {
    #pragma omp parallel for schedule(static, 256)
    for (size_t i = 0; i < data.size(); i++) {
        double minDist = std::numeric_limits<double>::max();
        int bestCluster = -1;
        for (int j = 0; j < k; j++) {
            double dist = euclideanDistance(data[i], centroids[j]);
            if (dist < minDist) {
                minDist = dist;
                bestCluster = j;
            }
        }
        data[i].cluster = bestCluster;
    }
}

// Update centroids
void updateCentroids(std::vector<Point>& data, std::vector<Centroid>& centroids, int k, int dimensions) {
    std::vector<std::vector<double>> sumCoords(k, std::vector<double>(dimensions, 0.0));
    std::vector<int> count(k, 0);

    // Create thread-local storage for reduction
    #pragma omp parallel
    {
        std::vector<std::vector<double>> localSumCoords(k, std::vector<double>(dimensions, 0.0));
        std::vector<int> localCount(k, 0);

        #pragma omp for nowait
        for (size_t i = 0; i < data.size(); i++) {
            int cluster = data[i].cluster;
            localCount[cluster]++;
            
            for (int d = 0; d < dimensions; d++) {
                localSumCoords[cluster][d] += data[i].coords[d];
            }
        }

        #pragma omp critical
        {
            for (int j = 0; j < k; j++) {
                count[j] += localCount[j];
                for (int d = 0; d < dimensions; d++) {
                    sumCoords[j][d] += localSumCoords[j][d];
                }
            }
        }
    }

    //#pragma omp parallel for
    for (int j = 0; j < k; j++) {
        if (count[j] > 0) {
            for (int d = 0; d < dimensions; d++) {
                centroids[j].coords[d] = sumCoords[j][d] / count[j];
            }
        }
    }
}

// K-Means clustering function
void kmeans(std::vector<Point>& data, int k, int max_iters = 20000) {
    int dimensions = data[0].coords.size();
    std::vector<Centroid> centroids(k);
    initializeCentroids(centroids, data, k);

    for (int iter = 0; iter < max_iters; iter++) {
        assignClusters(data, centroids, k);

        std::vector<Centroid> newCentroids = centroids;
        updateCentroids(data, newCentroids, k, dimensions);

        centroids = newCentroids;
    }

    std::cout << "Final cluster centers:\n";
    for (int i = 0; i < k; i++) {
        std::cout << "Cluster " << i << ": (";
        for (size_t d = 0; d < centroids[i].coords.size(); d++) {
            std::cout << centroids[i].coords[d] << (d == centroids[i].coords.size() - 1 ? ")" : ", ");
        }
        std::cout << "\n";
    }
}

int main() {
    int k = 5;  // Number of clusters
    int dimensions;
//    std::vector<Point> data = loadCSV("mallClusters.csv", &dimensions, {0,1,2,5});
    std::vector<Point> data = loadCSV("housing_clean.csv", &dimensions, {0,1,9});
    
   
    if (data.empty()) {
        std::cerr << "Error loading data!" << std::endl;
        return -1;
    }

    omp_set_num_threads(omp_get_max_threads());
    // Start timing
    double start_time = get_time();
    kmeans(data, k);
    double end_time = get_time();
    
    // Calculate elapsed time
    double duration = end_time - start_time;
    printf("Training completed in %f seconds.\n", duration);

    return 0;
}
