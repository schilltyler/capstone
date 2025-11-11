# Makefile for Process Creation assignment sample submission

CROSS_COMPILE ?=
CC := $(if $(CROSS_COMPILE),$(CROSS_COMPILE)gcc,gcc)

SRCDIR := src
INCDIR := inc
OBJDIR := obj
BINDIR := bin

APP_SRCS := $(wildcard $(SRCDIR)/exec_*.c)
RUNTIME_SRCS := $(SRCDIR)/runtime.c
CSRCS := $(APP_SRCS) $(RUNTIME_SRCS)

APP_OBJS := $(APP_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
RUNTIME_OBJS := $(RUNTIME_SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
ASRCS := $(SRCDIR)/start.S
AOBJS := $(ASRCS:$(SRCDIR)/%.S=$(OBJDIR)/%.o)

COMMON_OBJS := $(OBJDIR)/start.o $(OBJDIR)/runtime.o
BINS := $(APP_SRCS:$(SRCDIR)/%.c=$(BINDIR)/%)

CFLAGS += -O2 -Wall -Wextra -std=c11 -ffreestanding -fno-builtin -fno-stack-protector -fPIE -I$(INCDIR)
LDFLAGS += -static-pie -nostdlib -Wl,--build-id=none

QEMU ?= qemu-aarch64
ROOTFS ?= /usr/aarch64-linux-gnu
TEST_CHILD ?= /bin/sh -c "printf 'STDOUT test\n'; printf 'STDERR test\n' 1>&2"

.PHONY: all clean submission.zip
.SECONDARY: $(APP_OBJS) $(RUNTIME_OBJS) $(AOBJS)

all: $(BINS)

$(BINDIR) $(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.S | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/exec_%: $(OBJDIR)/exec_%.o $(COMMON_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $(COMMON_OBJS) $<

.PHONY: test
test: all
	@command -v $(QEMU) >/dev/null || { echo "Missing $(QEMU). Set QEMU=/path/to/qemu-aarch64"; exit 1; }
	@[ -d "$(ROOTFS)" ] || { echo "Missing ROOTFS directory '$(ROOTFS)'. Set ROOTFS=/path/to/aarch64/rootfs"; exit 1; }
	@for bin in $(BINS); do \
		echo "[*] Testing $$bin"; \
		OUTPUT="$$( $(QEMU) -L $(ROOTFS) $$bin $(TEST_CHILD) )" || { echo "Execution failed for $$bin"; exit 1; }; \
		echo "$$OUTPUT" | grep -q "STDOUT test" || { echo "Missing stdout payload from $$bin"; exit 1; }; \
		echo "$$OUTPUT" | grep -q "STDERR test" || { echo "Missing stderr payload from $$bin"; exit 1; }; \
		echo "$$OUTPUT"; \
	done

clean:
	rm -rf $(OBJDIR) $(BINDIR)

submission.zip: $(CSRCS) $(ASRCS) inc/syscalls.h inc/runtime.h Makefile
	zip -r submission.zip src inc Makefile
