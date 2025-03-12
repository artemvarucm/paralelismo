#include <unordered_set>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "csv_dataset.h"

// Function to check if a string is a valid number
bool isValidNumber(const std::string& str) {
    if (str.empty()) return false;
    
    char* end = nullptr;
    strtod(str.c_str(), &end);
    return (*end == '\0');  // True if full string is converted
}

std::vector<Point> loadCSV(const std::string& filename, int *dimensions, const std::vector<int>& excludeCols) {
    std::vector<Point> data;
    std::ifstream file(filename);
    std::string line, word;

    // Read header line to determine column indices
    getline(file, line);
    std::stringstream header(line);
    std::vector<std::string> columnNames;
    std::unordered_set<int> excludeSet(excludeCols.begin(), excludeCols.end());

    int colIndex = 0;
    while (getline(header, word, ',')) {
        if (excludeSet.find(colIndex) == excludeSet.end()) {
            columnNames.push_back(word);
        }
        colIndex++;
    }

    *dimensions = colIndex - excludeCols.size(); // Total columns - excluded columns

    // Read data rows
    int i_line=0;
    // Read data rows
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::vector<double> coords;
        colIndex = 0;
        bool validRow = true;

        while (getline(ss, word, ',')) {
            word.erase(0, word.find_first_not_of(" \t"));
            word.erase(word.find_last_not_of(" \t") + 1);

            if (excludeSet.find(colIndex) == excludeSet.end()) {
                if (isValidNumber(word)) {
                    coords.push_back(stod(word));  // Convert to double safely
                } else {
                    std::cerr << "Warning: Invalid number '" << word << "' in column " 
                         << colIndex << ". Skipping row." << "Line:" << i_line << std::endl;
                    validRow = false;
                    break;  // Skip this row completely
                }
            }
            colIndex++;
        }
        if (validRow && (coords.size() == *dimensions)) {
            data.push_back({coords, -1});
        }
        i_line++;
    }

    std::cout << "Total valid data points: " << data.size() << std::endl;
    file.close();
    return data;
}

