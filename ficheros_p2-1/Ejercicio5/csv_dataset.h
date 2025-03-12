#ifndef CSV_DATASET_H
#define CSV_DATASET_H

// Function to read CSV file into X, y, and labels
typedef struct {
    float *X;  //[n_features]
    int y;
} Tsample;



typedef struct {
    Tsample *samples;
    char **labels;
    int n_features;
    int n_samples;
} CSVData;

CSVData read_csv(const char *filename);
void free_csv_data(CSVData data);

#endif

