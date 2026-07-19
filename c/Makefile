NAME = clox
SOURCE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD_DIR := $(SOURCE_DIR)/build
GEN_DIR := $(SOURCE_DIR)/gen
DEFS_DIR := $(dir $(realpath $(dir $SOURCE_DIR)))/defs
TOOLS_DIR := $(realpath $(dir $SOURCE_DIR))/tools

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

clean:
	rm -rf $(BUILD_DIR) $(GEN_DIR)

# Link interpreter
$(BUILD_DIR)/$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile object files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS) $(SOURCE_DIR)/Makefile $(GEN_DIR)/tokens.h $(GEN_DIR)/ident.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(GEN_DIR)/ident.h: $(DEFS_DIR)/tokens.def $(TOOLS_DIR)/ident.pl
	@mkdir -p $(dir $@)
	$(TOOLS_DIR)/ident.pl $< > $@

$(GEN_DIR)/tokens.h: $(DEFS_DIR)/tokens.def Makefile
	@mkdir -p $(dir $@)
	perl -lne 'BEGIN { print "// generated from Makefile.\n#ifndef CLOX_GEN_TOKENS_H\n#define CLOX_GEN_TOKENS_H\n\ntypedef enum {"; } if (/^\s*(#.*)?\z/){ s{\s*#}{  \/\/} if /\S/; print; } else { print "  TOKEN_$$_,"; } END { print "\n  TOKEN_ERROR\n} token_type_t;\n\n#endif" }' $< > $@
