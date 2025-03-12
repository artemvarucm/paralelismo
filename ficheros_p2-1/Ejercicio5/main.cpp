#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "csv_dataset.h"
#include "extra.h"

// Compute Euclidean distance
float euclideanDistance(float *a, float *b, int n_features) {
    float sum = 0.0f;
    for (int i = 0; i < n_features; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrtf(sum);
}

// K-Nearest Neighbors Classification
int classify(CSVData train_data, float *queryX, int k, float *distances, int *labels) {

    
    // Compute distances
    for (int i = 0; i < train_data.n_samples; i++) {
        distances[i] = euclideanDistance(train_data.samples[i].X, queryX, train_data.n_features);
        labels[i] = train_data.samples[i].y;
    }

    // Find k nearest neighbors using a selection approach
    for (int i = 0; i < k; i++) {
        int min_index = i;
        for (int j = i + 1; j < train_data.n_samples; j++) {
            if (distances[j] < distances[min_index]) {
                min_index = j;
            }
        }
        // Swap distances and labels
        float temp_dist = distances[i];
        distances[i] = distances[min_index];
        distances[min_index] = temp_dist;
        
        int temp_label = labels[i];
        labels[i] = labels[min_index];
        labels[min_index] = temp_label;
    }

    // Count label occurrences
    int label_count[10] = {0};
    for (int i = 0; i < k; i++) {
        label_count[labels[i]]++;
    }
    
    // Determine most frequent label
    int max_label = 0;
    for (int i = 1; i < 10; i++) {
        if (label_count[i] > label_count[max_label]) {
            max_label = i;
        }
    }


    return max_label;
}

// Evaluate model with precision and recall
void evaluate_model(float *precision, float *recall, Tsample* samples, int* y_pred, int n_samples) {
    int tp = 0, fp = 0, fn = 0;

    for (int i = 0; i < n_samples; ++i) {

        if (y_pred[i] == 1 && samples[i].y == 1) tp++;
        if (y_pred[i] == 1 && samples[i].y == 0) fp++;
        if (y_pred[i] == 0 && samples[i].y == 1) fn++;
    }

    *precision = (float)(tp) / (float)(tp + fp);
    *recall = (float)(tp) / (float)(tp + fn);
}

int main() {
    setbuf(stdout, NULL);

    // Read dataset
    CSVData data = read_csv("Datasets/KNN_Large_Dataset.csv");

    // Split dataset into training and testing
    CSVData train_data, test_data;
    train_test_split(data, &train_data, &test_data, 0.2f, 7);
    
    float *distances = (float *)malloc(train_data.n_samples * sizeof(float));
    int *labels = (int *)malloc(train_data.n_samples * sizeof(int));
    int *y_pred = (int *)malloc(train_data.n_samples * sizeof(int));
    
    int correct = 0;
    clock_t start = clock();
    
    // Classify each test sample
    int k = 4;
    for (int i = 0; i < test_data.n_samples; i++) {
        y_pred[i]= classify(train_data, test_data.samples[i].X, k, distances, labels);
    }
    
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    
    // Print results
    float precision, recall;
    evaluate_model(&precision, &recall, test_data.samples, y_pred, test_data.n_samples);
    printf("Precision: %.2f Recall: %.2f\n", precision, recall);
    printf("Testing (%d) completed in %f seconds.\n", test_data.n_samples, duration);


    // Free memory
    free_csv_data(data);
    free_csv_data(train_data);
    free_csv_data(test_data);
    
    free(distances);
    free(labels);
    
    return 0;
}

