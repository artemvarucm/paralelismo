#ifndef CSV_DATASET_H
#define CSV_DATASET_H

typedef struct {
    float **X; // lista de listas, X[columna][fila]
    int *y;
    char **labels;
    int n_features;
    int n_samples;
} CSVData;

CSVData read_csv(const char *filename);
void free_csv_data(CSVData data);

#endif

