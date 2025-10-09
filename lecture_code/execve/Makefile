
# Makefile for C/C++ projects with bin/src/inc structure
CC ?= gcc
CXX ?= g++
CFLAGS ?= -O2 -Wall -Wextra -std=c99
CXXFLAGS ?= -O2 -Wall -Wextra -std=c++17
LDFLAGS ?= -static

# Cross-compilation support
CROSS_COMPILE ?= aarch64-linux-gnu-
ifneq ($(CROSS_COMPILE),)
    CC := $(CROSS_COMPILE)gcc
    CXX := $(CROSS_COMPILE)g++
endif

# Directories
SRCDIR := src
INCDIR := inc
BINDIR := bin
OBJDIR := obj

# Program targets - auto-discover from src/*.c
PROGRAM_SRCS := $(wildcard $(SRCDIR)/*.c)
PROGRAMS := $(notdir $(PROGRAM_SRCS:.c=))
PROGRAM_TARGETS := $(addprefix $(BINDIR)/, $(PROGRAMS))


# Source files
CSRCS := $(wildcard $(SRCDIR)/*.c)
CXXSRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.cc $(SRCDIR)/*.cxx)
COBJS := $(CSRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CXXOBJS := $(CXXSRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) \
           $(CXXSRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o) \
           $(CXXSRCS:$(SRCDIR)/%.cxx=$(OBJDIR)/%.o)
OBJS := $(COBJS) $(CXXOBJS)

# Include directories
INCLUDES := -I$(INCDIR)

# Default target
.PHONY: all clean $(PROGRAMS)

# Build all programs
all: $(PROGRAM_TARGETS)

# Individual program targets
$(PROGRAMS): %: $(BINDIR)/%

# All programs use start.S bootstrap for pure syscall programming
$(BINDIR)/start.o: $(SRCDIR)/start.S | $(BINDIR)
	$(CROSS_COMPILE)as $(SRCDIR)/start.S -o $@

# Generic rule for all programs
$(BINDIR)/%: $(SRCDIR)/%.c $(BINDIR)/start.o | $(BINDIR)
	$(CC) -nostdlib -O0 -fPIE -static -e _start $(INCLUDES) $(BINDIR)/start.o $< -o $@

# Create directories
$(BINDIR) $(OBJDIR):
	mkdir -p $@


# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

