CC = clang
CCFLAGS = -ansi -I$(INC_DIR)

INC_DIR = include
SRC_DIR = src
BUILD_DIR = build

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

TARGET_SRC = mangen.c
TARGET_OBJ = $(BUILD_DIR)/mangen.o
TARGET_BIN = mangen

all: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET_OBJ) $(OBJ_FILES)
	$(CC) $(CCFLAGS) $^ -o $@

make_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c make_dir
	$(CC) $(CCFLAGS) -c $< -o $@

$(TARGET_OBJ): $(TARGET_SRC) make_dir
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET_BIN)

.PHONY:
	all make_dir clean