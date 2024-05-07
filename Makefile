SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

CC = gcc
CFLAGS = -Wall -g -Wextra -pedantic -I$(INC_DIR)

# get all source and header files
SRC = $(wildcard $(SRC_DIR)/*.c)
HDR = $(wildcard $(INC_DIR)/*.h)

# create object files from source files
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXE=fetchmail

all: $(EXE)

# link object files to executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# create object files before linking
$(OBJ): | $(OBJ_DIR)

# ensure the object dir exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXE)

format:
	clang-format -style=file -i $(SRC_DIR)/*.c $(INC_DIR)/*.h