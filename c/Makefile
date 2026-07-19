NAME = clox
SOURCE_DIR = $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD_DIR = $(SOURCE_DIR)/build

# Files.
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

CFLAGS += -fsanitize=address,undefined -g -Wall -Wextra -Wpedantic -Werror
LDFLAGS += -fsanitize=address,undefined #-Wl,--as-needed

HAVE_READLINE := $(shell printf '\#include <readline/readline.h>\nint main(){readline("");return 0;}\n' | $(CC) -xc - -lreadline -o /dev/null 2>/dev/null && echo yes)

ifneq ($(HAVE_READLINE),yes)
  READLINE := $(shell brew --prefix readline)
  ifneq ($(READLINE),)
    CPPFLAGS += -I$(READLINE)/include
    LDFLAGS += -L$(READLINE)/lib
  endif
endif

ifeq ($(HAVE_READLINE),yes)
  CPPFLAGS += -DHAVE_READLINE
  LDLIBS += -lreadline
endif

.PHONY: all
all: $(BUILD_DIR)/$(NAME)

# Link interpreter
$(BUILD_DIR)/$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile object files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
