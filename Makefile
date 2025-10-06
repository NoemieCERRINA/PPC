SRC_DIR = src
OBJ_DIR = obj

CXXFLAGS = -std=c++14 -m64 -g -I$(SRC_DIR) -I$(INC) -Wall -Wextra
CXX = g++
DBGFLAGS = -g -DAPM_DEBUG
LDFLAGS = -g

OBJ_RAW = Constraint CSP utils
OBJ_O = $(addsuffix .o,$(OBJ_RAW))
OBJ = $(addprefix $(OBJ_DIR)/,$(OBJ_O))

EXEC = solver

all: $(OBJ_DIR) $(EXEC)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^ $(CPPLIB) -lm

solver: $(OBJ) $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(CPPLIB) -lm

clean:
	rm -rf $(EXEC) $(OBJ_DIR)