# powturbo  (c) Copyright 2013-2017
CC ?= gcc
CXX ?= g++
#CC=clang
#CXX=clang++

ifneq (,$(filter Windows%,$(OS)))
  OS := Windows
CFLAGS+=-D__int64_t=int64_t
else
  OS := $(shell uname -s)
  ARCH := $(shell uname -m)
ifneq (,$(findstring powerpc64le,$(CC)))
  ARCH = ppc64le
endif
ifneq (,$(findstring aarch64,$(CC)))
  ARCH = aarch64
endif
endif

#------ ARMv8 
ifeq ($(ARCH),aarch64)
ifneq (,$(findstring clang, $(CC)))
MSSE=-O3 -march=armv8-a -mcpu=cortex-a72 -falign-loops -fomit-frame-pointer 
else
MSSE=-O3 -march=armv8-a -mcpu=cortex-a72 -falign-loops -falign-labels -falign-functions -falign-jumps -fomit-frame-pointer
endif

else
# ----- Power9
ifeq ($(ARCH),ppc64le)
MSSE=-D__SSE__ -D__SSE2__ -D__SSE3__ -D__SSSE3__
MARCH=-march=power9 -mtune=power9
CFLAGS+=-DNO_WARN_X86_INTRINSICS
CXXFLAGS+=-DNO_WARN_X86_INTRINSICS
#------ x86_64 : minimum SSE = Sandy Bridge,  AVX2 = haswell 
else
MSSE=-march=corei7-avx -mtune=corei7-avx
# -mno-avx -mno-aes (add for Pentium based Sandy bridge)
CFLAGS+=-mssse3
MAVX2=-march=haswell
endif
endif

CFLAGS+=$(MSSE) $(MAVX2) -w

all: turbohist

turbohist: turbohist.o
	$(CC) $^ $(LDFLAGS) -o turbohist
 
.c.o:
	$(CC) -O3 $(CFLAGS) $< -c -o $@

clean:
	rm  *.o

cleanw:
	del .\*.o
