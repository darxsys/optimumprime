CC = g++
LD = g++

NAME = optimumprime

OBJ_DIR = obj
SRC_DIR = src

CP_FLAGS = -c -std=c++0x -Wall -Wno-write-strings -O3
LD_FLAGS = 

SRC = $(shell find $(SRC_DIR) -type f -regex ".*\.cpp")
OBJ = $(subst $(SRC_DIR), $(OBJ_DIR), $(addsuffix .o, $(basename $(SRC))))

EXC = $(NAME)

all: $(EXC)

$(EXC): $(OBJ)
	@echo [LD] $@
	@mkdir -p $(dir $@)
	@$(LD) -o $@ $^ $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [CP] $<
	@mkdir -p $(dir $@)
	@$(CC) -o $@ $^ $(CP_FLAGS)

clean:
	@echo [RM] cleaning
	@rm $(OBJ_DIR) $(EXC) -rf
