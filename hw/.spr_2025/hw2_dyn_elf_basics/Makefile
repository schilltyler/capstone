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
TARGETS := challenge dyn_printf find_lib hello run_printf

.PHONY: all clean dist 

all: $(TARGETS)

# 'challenge' is built from challenge.c and challenge.S.
challenge:
	$(CC) -nostdlib  $(SRC_DIR)/challenge.c  -o $(BIN_DIR)/challenge

challenge_asm.o: $(SRC_DIR)/challenge.S
	$(CC) -nostdlib -c -o $@ $<



# 'dyn_printf' is built from dyn_printf.c.
dyn_printf: $(SRC_DIR)/dyn_printf.c
	$(CC) $(CFLAGS) -o $@ $<

# 'find_lib' is built from find_lib.c.
find_lib: $(SRC_DIR)/find_lib.c
	$(CC) $(CFLAGS) -o $@ $<

# 'hello' is built from hello.c.
hello: $(SRC_DIR)/hello.c
	$(CC) $(CFLAGS) -o $@ $<

# 'run_printf' is built from run_printf.c plus its supporting modules.
run_printf:$(SRC_DIR)/run_printf.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)





# Make student 
dist:
	cp sol/src/*.c src/ 
	bash remove_sol.sh src/*.c 
	tar czf dist/dyn_elf.tar.gz src/ Makefile inc/ 

bundle:
	tar czf bundle.tar.gz src/ Makefile inc/ 
