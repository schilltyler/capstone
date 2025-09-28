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


# Default target: build all binaries
.PHONY: all clean

all:  bin  memfd_bin test_memfd_reflective

LOADER_FLAGS=-c  -Iinc -fPIC -O0 -static -shared  -nostdlib -fomit-frame-pointer  -fno-stack-protector  -fno-asynchronous-unwind-tables -nostartfiles -nostdlib 
bin:
	mkdir -p bin 
	

# build object file 
example_pic.o: bin
	# optionally merge rodata into .text 
	#aarch64-linux-gnu-objcopy --rename-section .rodata=.text bin/example_pic.o
	$(CC) $(LOADER_FLAGS) src/example_pic.c  \
		-o bin/example_pic.o

# link into shared object 
example_pic: example_pic.o
	$(LD) -shared  bin/example_pic.o -e sample -o bin/sample.so



reflective_loader.o: bin
	$(CC) $(LOADER_FLAGS) src/reflective_loader.c  \
		-o bin/reflective_loader.o 


reflective_payload: reflective_loader.o
	# optional merge .text and .rodata
	#aarch64-linux-gnu-objcopy --rename-section .rodata=.text bin/reflective_memfd_loader.o
	$(CC) -Iinc -fPIC -shared  \
		bin/reflective_loader.o \
		src/reflective_payload.c \
		-Wl,-entry=reflective_loader\
		-o bin/rp.so 

reflective_memfd_loader.o: bin
	$(CC) -c  $(LOADER_FLAGS) \
		src/reflective_memfd.c  \
		-o bin/reflective_memfd_loader.o

reflective_payload_memfd: reflective_memfd_loader.o
	# optional merge .text and .rodata
	#aarch64-linux-gnu-objcopy --rename-section .rodata=.text bin/reflective_memfd_loader.o
	$(CC) -Iinc -fPIC -shared  \
		bin/reflective_memfd_loader.o \
		src/reflective_memfd_payload.c \
		-Wl,-entry=load_memfd \
		-o bin/rp.so 

memfd_bin: reflective_payload_memfd
	python3 scripts/memfd_bootstrap.py bin/rp.so > bin/bootstrap.S
	aarch64-linux-gnu-as bin/bootstrap.S -o bin/bootstrap.o 
	aarch64-linux-gnu-objcopy -O binary -j .text bin/bootstrap.o bin/bootstrap.bin
	cat bin/bootstrap.bin bin/rp.so > bin/mempayload   

test_memfd_reflective:
	$(CC) src/test_memfd_reflective.c -o bin/test_memfd_reflective

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

