// icpx cnn.cpp -o cnn -lpng
// g++ cnn.cpp -o cnn -lpng
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cmath>

/* For timming */
#include <sys/time.h>  // gettimeofday
#include <unistd.h>    // for usleep (demo delay)

using namespace std;


double get_ms()
{
    timeval actual_time;
    gettimeofday(&actual_time, nullptr);

    long seconds = actual_time.tv_sec;
    long microseconds = actual_time.tv_usec;
    double elapsed_ms = seconds * 1000.0 + microseconds / 1000.0;

    return(elapsed_ms);
}

// Helper to read weights
float* load_weights(const string& filename, int* size) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open weights file: " << filename << endl;
        exit(1);
    }

    vector<float> temp;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        while (getline(ss, val, ',')) {
            if (!val.empty()) {
                temp.push_back(stof(val));
            }
        }
    }

    *size = temp.size();
    float* data = new float[*size];
    for (int i = 0; i < *size; ++i) {
        data[i] = temp[i];
    }

    return data;
}
#pragma omp declare target
void conv2d(float* input, float* output, float* kernel, int height, int width, float bias) {
    #pragma omp target teams distribute parallel for collapse(2)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            float sum = 0.0f;
            for (int ki = -1; ki <= 1; ++ki) {
                for (int kj = -1; kj <= 1; ++kj) {
                    int ni = i + ki;
                    int nj = j + kj;
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        float val = input[ni * width + nj];
                        float k = kernel[(ki + 1) * 3 + (kj + 1)];
                        sum += val * k;
                    }
                }
            }
            output[i * width + j] = max(0.0f, sum + bias); // ReLU activation
        }
    }
}
#pragma omp end declare target

#pragma omp declare target
// Max Pooling 2x2 with stride=2
void maxpool2d(float* input, float* output, int height, int width) {
    #pragma omp target teams distribute parallel for collapse(2)
    for (int i = 0; i < height/2; ++i) {
        for (int j = 0; j < width/2; ++j) {
            float max_val = -3.4e38;

            for (int ki = 0; ki < 2; ++ki) {
                for (int kj = 0; kj < 2; ++kj) {
                    int ni = i * 2 + ki;
                    int nj = j * 2 + kj;
                    if (ni < height && nj < width) {
                        int idx = ni * width + nj;
                        max_val = max(max_val, input[idx]);
                    }
                }
            }
            output[i * width/2 + j] = max_val;
        }
    }
}
#pragma omp end declare target

#pragma omp declare target
// Fully connected layer
void fully_connected(float* input, float* weights, float* biases, float* output, int fc_output_size, int fc_input_size) {
    #pragma omp target teams distribute parallel for
    //Mapear los datos una sola vez
        for (int i = 0; i < fc_output_size; i++) {
            float sum = 0.0f;
            for (int j = 0; j < fc_input_size; j++) {
                sum += input[j] * weights[i * fc_input_size + j];
            }
            output[i] = sum + biases[i];
        }
    }
#pragma omp end declare target
// Load and normalize image to [-1, 1], return 1D vector
#include <png.h>

float* load_image(const string& filename, int* width_out, int* height_out) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        cerr << "Failed to open PNG file: " << filename << endl;
        exit(1);
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (!png || !info) {
        cerr << "Failed to create PNG read struct" << endl;
        fclose(fp);
        exit(1);
    }

    if (setjmp(png_jmpbuf(png))) {
        cerr << "PNG read error" << endl;
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        exit(1);
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGBA)
        png_set_rgb_to_gray_fixed(png, 1, -1, -1);
    if (bit_depth == 16)
        png_set_strip_16(png);
    if (bit_depth < 8)
        png_set_packing(png);
    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_strip_alpha(png);

    png_read_update_info(png, info);

    vector<png_bytep> row_pointers(height);
    vector<unsigned char> image_data(width * height);

    for (size_t i = 0; i < height; ++i)
        row_pointers[i] = &image_data[i * width];

    png_read_image(png, row_pointers.data());

    fclose(fp);
    png_destroy_read_struct(&png, &info, nullptr);

    // Output size from image
    *width_out = static_cast<int>(width);
    *height_out = static_cast<int>(height);

    // Allocate and normalize image to float [-1, 1]
    float* output = new float[width * height];
    for (int i = 0; i < static_cast<int>(height); ++i) {
        for (int j = 0; j < static_cast<int>(width); ++j) {
            unsigned char pixel = image_data[i * width + j];
            output[i * width + j] = static_cast<float>(pixel) / 255.0f * 2.0f - 1.0f;
        }
    }

    return output;
}

vector<float> softmax(float *logits, int size) {
    vector<float> probs(size);
    float max_logit = logits[0];
    for (int i = 1; i < size; ++i) {
        if (logits[i] > max_logit) {
            max_logit = logits[i];
        }
    }
    float sum = 0.0f;
    for (int i = 0; i < size; ++i) {
        probs[i] = exp(logits[i] - max_logit);
        sum += probs[i];
    }
    for (int i = 0; i < size; ++i) {
        probs[i] /= sum;
    }
    return probs;
}

int main() {
    // Load weights and biases
    int size_conv1_weights, size_conv1_biases, size_fc1_weights, size_fc1_biases;
    float* conv1_weights = load_weights("conv1_weights.txt", &size_conv1_weights);
    float* conv1_biases = load_weights("conv1_biases.txt", &size_conv1_biases);
    float* fc1_weights = load_weights("fc1_weights.txt", &size_fc1_weights);
    float* fc1_biases = load_weights("fc1_biases.txt", &size_fc1_biases);
    int fc_input_size = size_fc1_weights/size_fc1_biases;

    // Load image from PNG
    int width, height;
    float* input = load_image("mnist7.png", &width, &height);
    if (width!=height){
        printf("Image not square!!\n");
        exit(-1);
    }

    // Auxiliary memory for the CNN processing
    float* conv_out = new float[width*height];
    float* pooled = new float[(width/2)*(height/2)];
    float* output = new float[size_fc1_biases];


    // Get start time
    double t0 = get_ms();

    ///////////////////////////////////////////////////////////////////// 
    // Forward pass       
    #pragma omp target enter data map(to:input[:width*height])
    #pragma omp target enter data map(alloc:conv_out[:width*height])
    #pragma omp target enter data map(to:conv1_weights[:size_conv1_weights])
    #pragma omp target enter data map(to:conv1_biases[:size_conv1_biases])
    #pragma omp target enter data map(to:fc1_weights[:fc_input_size*size_fc1_weights])
    #pragma omp target enter data map(to:fc1_biases[:size_fc1_biases])

    conv2d(input, conv_out, conv1_weights, height, width, *conv1_biases);
    #pragma omp target exit data map(delete:conv1_weights[:size_conv1_weights])
    #pragma omp target exit data map(delete:conv1_biases[:size_conv1_biases])

    #pragma omp target enter data map(alloc:pooled[:(width/2)*(height/2)])
    maxpool2d(conv_out, pooled, height, width);

    #pragma omp target enter data map(alloc:output[:size_fc1_biases])
    fully_connected(pooled, fc1_weights, fc1_biases, output, size_fc1_biases, fc_input_size);
    #pragma omp target exit data map(delete:fc1_weights[:size_fc1_weights])
    #pragma omp target exit data map(delete:fc1_biases[:size_fc1_biases])
    #pragma omp target exit data map(delete:conv_out[:width*height])
    #pragma omp target exit data map(delete:pooled[:(width/2)*(height/2)])
    #pragma omp target exit data map(from:output[:size_fc1_biases])

    // Get end time
    double t1 = get_ms();
    cout << "Execution time: " << t1-t0 << " ms" << endl << endl;
    ///////////////////////////////////////////////////////////////////// 

    vector<float> probabilities = softmax(output, size_fc1_biases);
    cout << "Class probabilities:" << endl;
    for (int i = 0; i < probabilities.size(); ++i)
        cout << "   Class " << i << ": " << probabilities[i] << endl;

    // Find predicted class
    int pred = 0;
    float max_val = output[0];
    for (int i = 1; i < size_fc1_biases; ++i) {
        if (output[i] > max_val) {
            max_val = output[i];
            pred = i;
        }
    }
    cout << "Predicted class: " << pred << endl;

    return 0;
}