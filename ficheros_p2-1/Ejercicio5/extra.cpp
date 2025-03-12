#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "extra.h"

void train_test_split(CSVData data, CSVData *train, CSVData *test, float test_size, int random_state) {
    // Extract values of data
    int n_samples = data.n_samples;
    int n_features = data.n_features;

    int test_count = (int)(n_samples * test_size);
    int train_count = n_samples - test_count;

    train->n_features = test->n_features = n_features;
    train->n_samples = train_count;
    test->n_samples = test_count;

    // Allocate and copy labels
    train->labels = (char **)malloc((n_features + 1) * sizeof(char *));
    test->labels = (char **)malloc((n_features + 1) * sizeof(char *));
    for (int i = 0; i < n_features; i++) {
        train->labels[i] = strdup(data.labels[i]);
        test->labels[i] = strdup(data.labels[i]);
    }
    
    train->samples = (Tsample*)malloc(train->n_samples * sizeof(Tsample));
    test->samples  = (Tsample*)malloc(test->n_samples  * sizeof(Tsample));

    // Initialize random seed
    srand(random_state);

    int *indices = (int *)malloc(n_samples * sizeof(int));
    for (int i = 0; i < n_samples; i++) {
        indices[i] = i;
    }

    // Shuffle indices
    for (int i = n_samples - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    // Split data into training and testing sets
    for (int i = 0; i < train_count; i++) {
        int idx = indices[i];
        train->samples[i].X = (float *)malloc(n_features*sizeof(float*));
        for (int j = 0; j < n_features; j++) {
            train->samples[i].X[j] = data.samples[idx].X[j];
        }
        train->samples[i].y = data.samples[idx].y;
    }

    for (int i = 0; i < test_count; i++) {
        int idx = indices[train_count + i];
        test->samples[i].X = (float *)malloc(n_features*sizeof(float*));
        for (int j = 0; j < n_features; j++) {
            test->samples[i].X[j] = data.samples[idx].X[j];
        }
        test->samples[i].y = data.samples[idx].y;
    }
    free(indices);
}

