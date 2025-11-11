# Makefile for ELF Loading Assignment
CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
ASFLAGS ?=

# Cross-compilation support
CROSS_COMPILE ?= aarch64-linux-gnu-
ifneq ($(CROSS_COMPILE),)
    CC := $(CROSS_COMPILE)gcc
    AS := $(CROSS_COMPILE)as
endif

# Directories
SRCDIR := src
INCDIR := inc
BINDIR := bin
OBJDIR := obj

# Compiler flags for nostdlib and static-pie
CFLAGS += -nostdlib -static-pie -fPIC -fno-stack-protector -I$(INCDIR)
LDFLAGS := -nostdlib -static-pie -Wl,-e,_start

# Common object files (needed by all programs)
COMMON_OBJS := $(OBJDIR)/start.o $(OBJDIR)/utils.o $(OBJDIR)/elf_utils.o

# Programs to build
PROGRAMS := debug_elf_header validate_elf debug_program_headers debug_segments mini_loader sample hello_world

# All binaries
BINARIES := $(addprefix $(BINDIR)/,$(PROGRAMS))

# Default target
.PHONY: all clean submission.zip test

all: $(BINARIES)

# Create directories
$(BINDIR) $(OBJDIR):
	mkdir -p $@

# Build start.o from assembly
$(OBJDIR)/start.o: $(SRCDIR)/start.S | $(OBJDIR)
	$(CC) $(ASFLAGS) -c $< -o $@

# Build utils.o
$(OBJDIR)/utils.o: $(SRCDIR)/utils.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build elf_utils.o
$(OBJDIR)/elf_utils.o: $(SRCDIR)/elf_utils.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build debug_elf_header
$(BINDIR)/debug_elf_header: $(OBJDIR)/debug_elf_header.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/debug_elf_header.o: $(SRCDIR)/debug_elf_header.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build validate_elf
$(BINDIR)/validate_elf: $(OBJDIR)/validate_elf.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/validate_elf.o: $(SRCDIR)/validate_elf.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build debug_program_headers
$(BINDIR)/debug_program_headers: $(OBJDIR)/debug_program_headers.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/debug_program_headers.o: $(SRCDIR)/debug_program_headers.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build debug_segments
$(BINDIR)/debug_segments: $(OBJDIR)/debug_segments.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/debug_segments.o: $(SRCDIR)/debug_segments.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build mini_loader
$(BINDIR)/mini_loader: $(OBJDIR)/mini_loader.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/mini_loader.o: $(SRCDIR)/mini_loader.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build sample (test binary)
$(BINDIR)/sample: $(OBJDIR)/sample.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/sample.o: $(SRCDIR)/sample.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build hello_world (minimal test binary with only syscalls)
$(BINDIR)/hello_world: $(OBJDIR)/hello_world.o | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/hello_world.o: $(SRCDIR)/hello_world.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Package submission for Gradescope
submission.zip: $(SRCDIR)/*.c $(SRCDIR)/*.S $(INCDIR)/*.h Makefile
	zip -r submission.zip src inc Makefile

# Test target: run the mini_loader with hello_world test program
test: $(BINDIR)/mini_loader $(BINDIR)/hello_world
	@echo "Running test: mini_loader loading hello_world..."
	@echo "=============================================="
	$(BINDIR)/mini_loader $(BINDIR)/hello_world
	@echo "=============================================="
	@echo "Test completed successfully!"
