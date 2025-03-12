#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "csv_dataset.h"
#include "extra.h"

// Compute Euclidean distance
double euclideanDistance(double *a, double *b, int n_features) {
    double sum = 0.0f;
    for (int i = 0; i < n_features; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrtf(sum);
}

// K-Nearest Neighbors Classification
int classify(CSVData train_data, double *queryX, int k, double *distances, int *labels) {

    for (int i = 0; i < train_data.n_samples; i++) {
        distances[i] = 0;
        labels[i] = train_data.y[i];
    }
    
    // Compute distances
    for (int j = 0; j < train_data.n_features; j++) {
        double *column = train_data.X[j];
        for (int i = 0; i < train_data.n_samples; i++) {
            double diff = column[i] - queryX[j];
            distances[i] += diff * diff;
            //distances[i] = euclideanDistance(train_data.X[j], queryX, train_data.n_features);
            //labels[i] = train_data.y[i];
        }
    }

    for (int i = 0; i < train_data.n_samples; i++) {
        distances[i] = sqrtf(distances[i]);
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
        double temp_dist = distances[i];
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
void evaluate_model(double *precision, double *recall, int* y_true, int* y_pred, int n_samples) {
    int tp = 0, fp = 0, fn = 0;

    for (int i = 0; i < n_samples; ++i) {

        if (y_pred[i] == 1 && y_true[i] == 1) tp++;
        else if (y_pred[i] == 1 && y_true[i] == 0) fp++;
        else if (y_pred[i] == 0 && y_true[i] == 1) fn++;
    }

    *precision = (double)(tp) / (double)(tp + fp);
    *recall = (double)(tp) / (double)(tp + fn);
}

int main() {
    setbuf(stdout, NULL);

    // Read dataset
    CSVData data = read_csv("Datasets/KNN_Large_Dataset.csv");

    // Split dataset into training and testing
    CSVData train_data, test_data;
    train_test_split(data, &train_data, &test_data, 0.2f, 7);
    
    double *distances = (double *)malloc(train_data.n_samples * sizeof(double));
    int *labels = (int *)malloc(train_data.n_samples * sizeof(int));
    int *y_pred = (int *)malloc(train_data.n_samples * sizeof(int));
    
    int correct = 0;
    clock_t start = clock();
    
    // Classify each test sample
    int k = 4;
    for (int i = 0; i < test_data.n_samples; i++) {
        double *X_test = (double *)malloc(test_data.n_features * sizeof(double));
        for (int j = 0; j < test_data.n_features; j++) {
            X_test[j] = test_data.X[j][i];
        }
        
        y_pred[i]= classify(train_data, X_test, k, distances, labels);
    }
    
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    
    // Print results
    double precision, recall;
    evaluate_model(&precision, &recall, test_data.y, y_pred, test_data.n_samples);
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

