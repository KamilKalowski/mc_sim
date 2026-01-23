CXX      := clang++
CXXFLAGS := -std=c++17 -O2 -I.

SRC         := src/monte_carlo_cpu.cpp
BUILD_DIR   := build
TARGET      := $(BUILD_DIR)/monte_carlo
OUTPUT      := output

.PHONY: all run clean clear_output

all: $(TARGET)

$(TARGET): $(SRC) utils/xoshiro256ss.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

clear_output:
	@mkdir -p $(OUTPUT)
	find $(OUTPUT) -mindepth 1 -delete