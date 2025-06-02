
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iheader -g

BUILD_DIR := build
SRC_DIR := src
HEADER_DIR := header
TEST_EXEC := structure_test

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

GTEST_FLAGS := -lgtest -lgtest_main -pthread

.PHONY: all test clean

all: $(BUILD_DIR)/$(TEST_EXEC)

test: $(BUILD_DIR)/$(TEST_EXEC)
	@echo "Running tests..."
	@./$<

clean:
	@echo "Cleaning build..."
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_DIR)/*.h | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TEST_EXEC): $(OBJS)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_FLAGS)