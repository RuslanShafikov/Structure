CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -Iheader
LDFLAGS := -lgtest -lpthread

TARGET := skip_list_test
BUILD_DIR := build

SOURCES := src/maintest.cpp
OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))
DEPENDS := $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/maintest.o: src/maintest.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

test: $(TARGET)
	./$(TARGET)

-include $(DEPENDS)

.PHONY: all clean test