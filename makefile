# Makefile for compiling sintatico.cpp with and without debug mode

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Debug mode flag
DEBUG_FLAG = -DDEBUG -ggdb3

# Source file and executable name
SRC = sintatico.cpp
EXE = sintatico

# Output directory
OUT_DIR = out

# Ensure the output directory exists
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Debug and release targets
debug: $(OUT_DIR)/$(EXE)_debug

release: $(OUT_DIR)/$(EXE)

# Executable with debug mode
$(OUT_DIR)/$(EXE)_debug: $(SRC) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAG) $< -o $@ -MMD -MP

# Executable without debug mode
$(OUT_DIR)/$(EXE): $(SRC) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ -MMD -MP

# Clean target
clean:
	rm -f $(OUT_DIR)/$(EXE) $(OUT_DIR)/$(EXE)_debug
