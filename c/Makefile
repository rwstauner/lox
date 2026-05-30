NAME = clox
SOURCE_DIR = $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD_DIR = $(SOURCE_DIR)/build

# Files.
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

.PHONY: all
all: $(BUILD_DIR)/$(NAME)

# Link interpreter
$(BUILD_DIR)/$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile object files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(C_LANG) $(CFLAGS) -o $@ $<
