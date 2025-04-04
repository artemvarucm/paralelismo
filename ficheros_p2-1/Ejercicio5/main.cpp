#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "csv_dataset.h"
#include "extra.h"
#include <immintrin.h>


// Compute Euclidean distance
/*
float euclideanDistance(float *a, float *b, int n_features) {
    float sum = 0.0f;
    for (int i = 0; i < n_features; i++) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrtf(sum);
}
*/

// K-Nearest Neighbors Classification
int classify(CSVData train_data, float *queryX, int k, float *distances, int *labels) {

    for (int i = 0; i < train_data.n_samples; i++) {
        distances[i] = 0;
        labels[i] = train_data.y[i];
    }
    
    // Compute distances
    for (int j = 0; j < train_data.n_features; j++) {
        float *column = train_data.X[j];  
        
        __m128 sse_queryX, sse_train_data, sse_diff, sse_squared, sse_distances;
        sse_queryX = _mm_set1_ps(queryX[j]);

        int i;
        for (i = 0; i + 3 < train_data.n_samples; i += 4) {  
            sse_train_data = _mm_load_ps(&column[i]);  
            sse_diff = _mm_sub_ps(sse_train_data, sse_queryX);  
            sse_squared = _mm_mul_ps(sse_diff, sse_diff);  

            sse_distances = _mm_load_ps(&distances[i]);  
            sse_distances = _mm_add_ps(sse_distances, sse_squared);  
            _mm_storeu_ps(&distances[i], sse_distances);  
        }
        for (; i < train_data.n_samples; i++) {
            float diff = column[i] - queryX[j];
            distances[i] += diff * diff;
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
void evaluate_model(float *precision, float *recall, int* y_true, int* y_pred, int n_samples) {
    int tp = 0, fp = 0, fn = 0;

    for (int i = 0; i < n_samples; ++i) {
        if (y_pred[i] == 1 && y_true[i] == 1) tp++;
        else if (y_pred[i] == 1 && y_true[i] == 0) fp++;
        else if (y_pred[i] == 0 && y_true[i] == 1) fn++;
    }

    *precision = (float)(tp) / (float)(tp + fp);
    *recall = (float)(tp) / (float)(tp + fn);
}

int main() {
    setbuf(stdout, NULL);

    // Read dataset
    CSVData data = read_csv("./Datasets/KNN_Large_Dataset.csv");

    // Split dataset into training and testing
    CSVData train_data, test_data;
    train_test_split(data, &train_data, &test_data, 0.2f, 7);
    
    float *distances = (float *)malloc(train_data.n_samples * sizeof(float*));
    int *labels = (int *)malloc(train_data.n_samples * sizeof(int));
    int *y_pred = (int *)malloc(train_data.n_samples * sizeof(int));
    
    int correct = 0;
    clock_t start = clock();
    
    // Classify each test sample
    int k = 4;
    float *X_test = (float *)malloc(test_data.n_features * sizeof(float*));
    for (int i = 0; i < test_data.n_samples; i++) {
        for (int j = 0; j < test_data.n_features; j++) {
            X_test[j] = test_data.X[j][i];
        }
        
        y_pred[i] = classify(train_data, X_test, k, distances, labels);
    }
    free(X_test);
    
    clock_t end = clock();
    float duration = (float)(end - start) / CLOCKS_PER_SEC;
    
    // Print results
    float precision, recall;
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

