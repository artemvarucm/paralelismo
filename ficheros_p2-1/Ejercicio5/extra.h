#ifndef EXTRA_H
#define EXTRA_H

#include "csv_dataset.h"
void train_test_split(CSVData data, CSVData *train_data, CSVData *test_data, float test_size, int random_state);

#endif
