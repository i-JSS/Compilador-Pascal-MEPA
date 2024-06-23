CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11
DEBUG_FLAG = -DDEBUG -ggdb3

SRC = sintatico.cpp
EXE = sintatico

OUT_DIR = out

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

debug: $(OUT_DIR)/$(EXE)_debug

release: $(OUT_DIR)/$(EXE)

$(OUT_DIR)/$(EXE)_debug: $(SRC) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAG) $< -o $@ 

$(OUT_DIR)/$(EXE): $(SRC) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ 

clean:
	rm -f $(OUT_DIR)/$(EXE) $(OUT_DIR)/$(EXE)_debug
