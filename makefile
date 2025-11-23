TARGET := blastmap
BUILD_DIR := build

CSRCS := $(sort $(shell find src -name '*.c'))
CPPSRCS := $(sort $(shell find src -name '*.cpp'))

COBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
CXXOBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(CPPSRCS))

OBJS := $(COBJS) $(CXXOBJS)

CC := gcc
CXX := g++
CFLAGS := -std=gnu99 -fcommon -g -Wall
CXXFLAGS := -std=c++17 -fcommon -g -Wall
LDLIBS := -pthread -lX11 -lGL -lGLU

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
