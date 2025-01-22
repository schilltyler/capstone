# Variables
AS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld

SRC_DIR = ./src
BIN_DIR = ./bin
INCLUDE_DIR = ./src # Directory containing common.S
LDFLAGS = -lc# Add linker flags here
ASFLAGS = -I $(INCLUDE_DIR) # Include the directory for shared files

# Automatically discover all .S files in the source directory
SRC_FILES = $(wildcard $(SRC_DIR)/*.S)

# Generate a list of binaries based on the .S files
BINARIES = $(patsubst $(SRC_DIR)/%.S, %, $(SRC_FILES))

# Default target: show usage
all:
	@echo "Usage: make <target>"
	@echo "Available targets: $(BINARIES)"

# Create rules dynamically for each discovered binary
$(BIN_DIR)/%: $(SRC_DIR)/%.S
	@mkdir -p $(BIN_DIR)
	$(AS) $(ASFLAGS) -o $(BIN_DIR)/$*.o $<
	$(LD) $(LDFLAGS) -o $@ $(BIN_DIR)/$*.o

# Phony target to handle dynamic rules
$(BINARIES): %: $(BIN_DIR)/%

# Clean up
clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean $(BINARIES)

