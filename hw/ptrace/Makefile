# Compiler and Tools
ARCH := aarch64-linux-gnu
CC := $(ARCH)-gcc
LD := $(ARCH)-ld

# Directories
SRC_DIR := ./src
INC_DIR := ./inc
BUILD_DIR := ./build
BIN_DIR := ./bin

# Source files and corresponding object and binary files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
BINS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRCS))

# Compiler and linker flags
CFLAGS := -Wall -O0 -I$(INC_DIR)
LDFLAGS :=


# List of executables to build. In this example each file (or group) is built as its own target.
TARGETS := inject  
.PHONY: all clean dist 

all: $(TARGETS)

# 'challenge' is built from challenge.c and challenge.S.
inject:
	$(CC)  $(CFLAGS) $(SRC_DIR)/inject.c  -o $(BIN_DIR)inject

inject_sol:
	$(CC)  $(CFLAGS) sol/inject.c  -o $(BIN_DIR)inject

clean:
	rm -f $(TARGETS)



# Make student 
dist:
	tar czf dist/ptrace.tar.gz src/ Makefile inc/ 

bundle:
	tar czf bundle.tar.gz src/ Makefile inc/ 
