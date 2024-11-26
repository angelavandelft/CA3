#
# Skeleton code for use with Computer Architecture 2024 assignment 3,
# LIACS, Leiden University.
#
CC = gcc
CXX = g++

# Do not touch the "-f" options, these are here to help you not being misled by
# the compiler.
FLAGS = -O3 -g -Wall -Isupport \
	-fno-tree-vectorize -fno-unroll-loops -fno-inline

CFLAGS = $(FLAGS) -std=gnu99
CXXFLAGS = $(FLAGS) -std=c++17
LDFLAGS = -lm
PNGFLAGS = `pkg-config --cflags --libs libpng`

# Comment out these lines to disable the timing code. (macOS does not provide
# clock_gettime).
CFLAGS += -DENABLE_TIMING
CXXFLAGS += -DENABLE_TIMING
LDFLAGS += -lrt

# Add additional targets here:
TARGETS = \
	grayscale_xy \
	grayscale_yx \
	matrixmul-1-512 \
	matrixmul-50-512 \
	matrixmul-1-2048 \
	matrixmul-1-2048-8 \
	matrixmul-1-2048-10 \
	matrixmul-1-2048-16 \
	matrixmul-1-2048-32 \
	matrixmul-1-2048-64 \
	matrixmul-1-2048-128 \
	matrixmul-1-1024 \
	matrixmul-1-1024-8 \
	matrixmul-1-1024-16 \
	matrixmul-1-1024-32 \
	matrixmul-1-128 \
	matrixmul-1-128-8 \
	matrixmul-1-128-16 \
	matrixmul-1-128-32 \
	matrixmul-1-512 \
	matrixmul-1-512-8 \
	matrixmul-1-512-16 \
	matrixmul-1-512-32 \
	tilecomposite \
	postcode

# When you start investigating blocking, add a third number to the name of
# matrixmul for the block size (BLOCK).

# For example, matrixmul-1-512-32 specifies a block size of 32.

all: $(TARGETS)

grayscale_%: grayscale.c support/image.c support/timing.c
	$(CC) $(CFLAGS) -DVARIANT=$@ -o $@ $^ $(PNGFLAGS) $(LDFLAGS)

matrixmul-%: matrixmul.c support/timing.c
	@R=$$(echo $@ | cut -d '-' -f 2); \
	 N=$$(echo $@ | cut -d '-' -f 3); \
	 B=$$(echo $@ | cut -d '-' -f 4); \
	 $(CC) $(CFLAGS) -DN_REPEAT=$$R -DN=$$N -DBLOCK=$$B -o $@ $^ $(LDFLAGS)

tilecomposite: tilecomposite.c support/image.c support/timing.c
	$(CC) $(CFLAGS) -o $@ $^ $(PNGFLAGS) $(LDFLAGS)

postcode: postcode.cpp support/timing.c support/postcode.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGETS)
