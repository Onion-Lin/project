PROJECT_DIR := $(patsubst %/,%, $(dir $(lastword $(MAKEFILE_LIST))))
$(info "PROJECT DIRECTORY: $(PROJECT_DIR)")

C_FILES = $(shell find $(PROJECT_DIR) -name "*.c")
OBJ_DIR = $(PROJECT_DIR)/obj
OBJ_FILES = $(patsubst $(PROJECT_DIR)%.c, $(OBJ_DIR)/%.o, $(C_FILES))
CFLAGS = -Wall -Wextra -I$(PROJECT_DIR)/include -I$(PROJECT_DIR)/cpu/isa/include
LDFLAGS = $(shell pkg-config --libs sdl2)

$(OBJ_DIR)/%.o: $(PROJECT_DIR)%.c
	@mkdir -p $(@D)
	@gcc -c -o $@ $< $(CFLAGS)

EXEC = $(PROJECT_DIR)/minirvEMU

$(EXEC): $(OBJ_FILES)
	@echo GCC + $@
	@gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

BIN_PATH = $(PROJECT_DIR)/bin
HEX_FILES = $(shell find $(BIN_PATH) -name "*.hex")
BIN_FILES = $(patsubst $(BIN_PATH)%.hex, $(BIN_PATH)%.bin, $(HEX_FILES))
$(BIN_PATH)/%.bin: $(BIN_PATH)%.hex
	@echo HEX2BIN + $@
	@$(PROJECT_DIR)/hex2bin.sh $< $@

.PHONY: run clean clean-all

run: $(EXEC) $(BIN_FILES)
	@$(EXEC) $(BIN_PATH)/$(if $(arg),$(arg),mem).bin

clean:
	@rm -rf $(OBJ_DIR) $(EXEC)

clean-all:
	@rm -rf $(OBJ_DIR) $(EXEC) $(BIN_FILES)
