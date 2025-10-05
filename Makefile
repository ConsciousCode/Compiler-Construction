# Some of this taken from makefiletutorial.com

CXX = g++
LEVEL = -g
CXXFLAGS = -Wall -Wextra -Werror -fno-exceptions -fno-rtti $(LEVEL)

BUILD_DIR = build
SRC_DIR := src

SRCS := $(SRC_DIR)/test_scanner.cpp $(SRC_DIR)/scanner.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP

# Test Scanner Target
$(BUILD_DIR)/test_scanner: $(OBJS)
	$(CXX) $(OBJS) -o $@

# C++ Source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

all: $(BUILD_DIR)/test_scanner

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean all
