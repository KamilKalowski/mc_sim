OMP_CFLAGS  := -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include
OMP_LDFLAGS := -L/usr/local/opt/libomp/lib -lomp

CXX      := clang++
CXXFLAGS := -std=c++17 -O3 -ffast-math -march=native -I. $(OMP_CFLAGS)
LDFLAGS  := $(OMP_LDFLAGS)

SRC         := src/main.cpp
BUILD_DIR   := build
TARGET      := $(BUILD_DIR)/monte_carlo
OUTPUT      := output

.PHONY: all run clean clear_output

all: $(TARGET)

$(TARGET): $(SRC) utils/xoshiro256ss.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

clear_output:
	mkdir -p $(OUTPUT)
	find $(OUTPUT) -mindepth 1 -delete
