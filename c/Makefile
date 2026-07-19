NAME = clox
SOURCE_DIR = $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD_DIR = $(SOURCE_DIR)/build

# Files.
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

CFLAGS += -fsanitize=address,undefined -g -Wall -Wextra -Wpedantic -Werror
LDFLAGS += -fsanitize=address,undefined #-Wl,--as-needed

.PHONY: all
all: $(BUILD_DIR)/$(NAME)

# Link interpreter
$(BUILD_DIR)/$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile object files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
