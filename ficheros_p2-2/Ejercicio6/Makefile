CXX = icpx 
CXXFLAGS =  -g -O2 -qopenmp -xhost
TARGET = kmeans
LIBS = -lm

SRC = kmeans.cpp csv_dataset.cpp extra.cpp
OBJ = $(SRC:.cpp=.o)
HEADERS = csv_dataset.h extra.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *optrpt

