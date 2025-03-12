#include <sys/time.h>  // Include for gettimeofday()

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include <cstdlib>


// Function to get the current time in seconds
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1.0e6);  // Convert to seconds with microsecond precision
}
