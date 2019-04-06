COMMON_SRCS = $(wildcard src/common/*.c)
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

BIN_DIR = bin

GPU_PROG_ARCH = gpu-prog-arch
GPU_PROG_ARCH_SRCS = src/gpu_prog_arch.c $(wildcard src/commands/*)
GPU_PROG_ARCH_OBJS = $(GPU_PROG_ARCH_SRCS:.c=.o)

CLC = clc
CLC_SRCS = src/clc.c
CLC_OBJS = $(CLC_SRCS:.c=.o)

ASSEMBLY_DIR = assembly
ASSEMBLY_SRCS = $(wildcard $(ASSEMBLY_DIR)/*.s)
ASSEMBLY_BINS = $(ASSEMBLY_SRCS:.s=.sbin)

CL_DIR = cl
CL_SRCS = $(wildcard $(CL_DIR)/*.cl)
CL_BINS = $(CL_SRCS:.cl=.clbin)

OS = $(shell uname)

CC = gcc
CFLAGS = -std=gnu99 -O0 -g -Wall -pedantic -Wextra
LIBRARIES = -lm -ljpeg

M2C = ~/multi2sim-4.2/bin/m2c

ifeq ($(OS), Darwin)
	LIBRARIES += -framework OpenCL
else
	INCLUDES      += -I/opt/AMDAPP/include
	LIBRARY_PATHS += -L/opt/AMDAPP/lib/x86
	LIBRARIES     += -lOpenCL
endif

all: $(BIN_DIR) $(BIN_DIR)/$(GPU_PROG_ARCH) $(BIN_DIR)/$(CLC)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# TODO: Find M2S source.
ifneq ($(OS), Darwin)
assembly: $(ASSEMBLY_BINS)
	mkdir -p $(BIN_DIR)
	mv $^ $(BIN_DIR)

cl: $(CL_BINS)
	mkdir -p $(BIN_DIR)
	mv $^ $(BIN_DIR)
endif

$(BIN_DIR)/$(GPU_PROG_ARCH): $(COMMON_OBJS) $(GPU_PROG_ARCH_OBJS)
	$(CC) $(CFLAGS) $^ $(LIBRARY_PATHS) $(INCLUDES) $(LIBRARIES) -o $@

$(BIN_DIR)/$(CLC): $(COMMON_OBJS) $(CLC_OBJS)
	$(CC) $(CFLAGS) $^ $(LIBRARY_PATHS) $(INCLUDES) $(LIBRARIES) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.sbin: %.s
	$(M2C) --si2bin $< -o $@

%.clbin: %.cl
	$(M2C) --amd --amd-device Tahiti $< -o $@

clean:
	rm -f $(BIN_DIR)/* $(COMMON_OBJS) $(GPU_PROG_ARCH_OBJS) $(CLC_OBJS)
