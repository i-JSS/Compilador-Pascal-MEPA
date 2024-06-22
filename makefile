
# Makefile for compiling sintatico.cpp with and without debug mode

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Debug mode flag
DEBUG_FLAG = -DDEBUG -ggdb3

# Source file and executable name
SRC = sintatico.cpp
EXE = sintatico

# Debug and release targets
debug: $(EXE)_debug

release: $(EXE)

# Executable with debug mode
$(EXE)_debug: $(SRC)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAG) $< -o $@ -MMD -MP

# Executable without debug mode
$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ -MMD -MP

# Clean target
clean:
	rm -f $(EXE) $(EXE)_debug
