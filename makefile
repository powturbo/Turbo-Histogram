# powturbo  (c) Copyright 2013-2017
CC ?= gcc
CXX ?= g++
#CC=clang
#CXX=clang++

ifeq ($(OS),Windows_NT)
  UNAME := Windows
CC=gcc
CXX=g++
else
  UNAME := $(shell uname -s)
ifeq ($(UNAME),$(filter $(UNAME),Linux Darwin FreeBSD GNU/kFreeBSD))
#LDFLAGS+= -lrt
endif
endif

CFLAGS+=-march=native

all: turbohist

turbohist: turbohist.o
	$(CC) $^ $(LDFLAGS) -o turbohist
 
.c.o:
	$(CC) -O3 $(CFLAGS) $< -c -o $@

clean:
	rm  *.o

cleanw:
	del .\*.o
