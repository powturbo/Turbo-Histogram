# powturbo  (c) Copyright 2013-2022
# Download or clone Turbo-Histogram:
# git clone git://github.com/powturbo/Turbo-Histogram.git

#uncomment to enable
#https://github.com/nkurz/countbench (inline assembly)
#COUNTBENCH=1
#https://gist.github.com/rygorous/a86a5cf348922cdea357c928e32fc7e0  (delete/comment or rename main)
#RYG=1
# timer: rdtsc cycles/byte or wall time in MB/s
#RDTSC=1
#AVX2=1

#-------------------------------------------------------------------------------------
CC ?= gcc
CXX ?= g++
#CC=clang
#CXX=clang++
ASM ?= nasm

OPT=-fstrict-aliasing 
ifeq (,$(findstring clang, $(CC)))
OPT+=-falign-loops
endif

#------- OS/ARCH -------------------
ifneq (,$(filter Windows%,$(OS)))
  OS := Windows
#  CC=gcc
#  CXX=g++
  ARCH=x86_64
  LDFLAGS+=-Wl,--stack,8194304
  FASM=win64
else
  OS := $(shell uname -s)
  ARCH := $(shell uname -m)
  FASM=elf64

ifneq (,$(findstring aarch64,$(CC)))
  ARCH = aarch64
else ifneq (,$(findstring powerpc64le,$(CC)))
  ARCH = ppc64le
endif
endif

ifeq ($(ARCH),ppc64le)
  _SSE=-D__SSSE3__
  MARCH=-mcpu=power9 -mtune=power9 $(_SSE)
else ifeq ($(ARCH),aarch64)
  MARCH+=-march=armv8-a 
ifneq (,$(findstring clang, $(CC)))
  MARCH+=-march=armv8-a 
  OPT+=-fomit-frame-pointer
else
  MARCH+=-march=armv8-a 
endif
  SSE=-march=armv8-a
else ifeq ($(ARCH),$(filter $(ARCH),x86_64))
  LDFLAG+=-lm
# set minimum arch sandy bridge SSE4.1 + AVX
  _SSE=-march=corei7-avx -mtune=corei7-avx 
# SSE+=-mno-avx -mno-aes
  _AVX2=-march=haswell
#  CFLAGS=$(SSE)
#  CFLAGS=$(AVX2)
endif

ifeq ($(AVX2),1)
MARCH=$(_AVX2) 
else
MARCH=$(_SSE) 
endif

CFLAGS+=$(MARCH) -w $(OPT)
ifeq ($(STATIC),1)
LDFLAGS+=-static
endif

ifeq ($(RDTSC),1)
CFLAGS+=-D_RDTSC
endif

ifeq ($(COUNTBENCH),1)
CFLAGS+=-D_COUNTBENCH
endif

ifeq ($(RYG),1)
CFLAGS+=-D_RYG
ASMLIB=histo_asm.o
endif

all: turbohist 

histo_asm.o: histo_asm.nas
	$(ASM) -f $(FASM) histo_asm.nas -o histo_asm.o

turbohist: turbohist.o $(ASMLIB)
	$(CC) $^ $(LDFLAGS) -o turbohist

.c.o:
	$(CC) -O3 $(CFLAGS) $< -c -o $@


ifeq ($(OS),Windows)
clean:
	del /S *.o 
#	del /S *.exe
else
clean:
	find . -name "turbohist" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*~" -type f -delete
	find . -name "core" -type f -delete
endif
