CC = icpx
CFLAGS = -O3 -qopenmp -qopt-report=2

# Source files
SOURCES = main.cpp csv_dataset.cpp extra.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executables
EXEC = knn

# Default target
all: $(EXEC)

# Compile
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^


# Compile object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXEC) *optrpt
