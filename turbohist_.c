/**
Copyright (c) 2013-2019, Powturbo
    - homepage : https://sites.google.com/site/powturbo/
    - github   : https://github.com/powturbo
    - twitter  : https://twitter.com/powturbo
    - email    : powturbo [_AT_] gmail [_DOT_] com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include "conf.h"
  #ifdef __ARM_NEON
#define PREFETCH(_ip_,_rw_)
  #else
#define PREFETCH(_ip_,_rw_) __builtin_prefetch(_ip_,_rw_)
  #endif

#define RET  if(csum) { unsigned a=0, i; for(i = 0; i < 256; i++) a+=i*c[i];    return a;} else return c[0]
#define RETN if(csum) { unsigned a=0, i; for(i = 0; i < 256; i++) a+=i*c[0][i]; return a;} else return c[0][0]
static unsigned csum;

typedef unsigned bin_t;

unsigned hist_1_8(unsigned char *in, unsigned inlen) { 
  bin_t c[256] = {0};

  unsigned char *ip = in;
  while(ip < in+inlen)
    c[*ip++]++;
  RET;
}

unsigned hist_4_8(unsigned char *in, unsigned inlen) { 
  bin_t c[4][256]={0},i;

  unsigned char *ip; 
  for(ip = in; ip < in + (inlen&~(4-1)); ) 
    c[0][*ip++]++, c[1][*ip++]++, c[2][*ip++]++, c[3][*ip++]++; 
  while(ip < in+inlen) 
    c[0][*ip++]++; 

  for(i = 0; i < 256; i++) c[0][i] += c[1][i]+c[2][i]+c[3][i];
  RETN;
}

unsigned hist_8_8(unsigned char *in, unsigned inlen) { 
  bin_t c[8][256]={0},i; 

  unsigned char *ip; 
  for(ip = in; ip < in + (inlen&~(8-1)); ) 
    c[0][*ip++]++, c[1][*ip++]++, c[2][*ip++]++, c[3][*ip++]++, c[4][*ip++]++, c[5][*ip++]++, c[6][*ip++]++, c[7][*ip++]++; 
  while(ip < in+inlen) 
    c[0][*ip++]++; 

  for(i = 0; i < 256; i++) 
    c[0][i] += c[1][i]+c[2][i]+c[3][i]+c[4][i]+c[5][i]+c[6][i]+c[7][i];
  RETN;
}

//------------------------------------
#define CSIZE (256 + 8)

#define CU32(u,_i_) {\
  c[_i_+0][(unsigned char) u     ]++;\
  c[_i_+1][(unsigned char)(u>> 8)]++;\
  c[_i_+2][(unsigned char)(u>>16)]++;\
  c[_i_+3][ 	           u>>24 ]++;\
}

#define OV 8
#define INC4_32(_i_) { { unsigned u = ux, v = vx; ux = ctou32(ip+_i_+OV+0); vx = ctou32(ip+_i_+OV+ 4); CU32(u,0); CU32(v,0); }\
                       { unsigned u = ux, v = vx; ux = ctou32(ip+_i_+OV+8); vx = ctou32(ip+_i_+OV+12); CU32(u,0); CU32(v,0); }\
}

unsigned hist_4_32(unsigned char *in, unsigned inlen) {
  bin_t c[4][CSIZE] = {0},i; 

  unsigned char *ip = in; 
  if(inlen >= 64) {
    unsigned ux = ctou32(ip), vx = ctou32(ip+4);
    for(; ip != in+(inlen&~(64-1))-64; ip += 64) { INC4_32(0); INC4_32(16); INC4_32(32); INC4_32(48); __builtin_prefetch(ip+512, 0); }
  }
  while(ip != in+inlen) 
    c[0][*ip++]++; 

  for(i = 0; i < 256; i++)
    c[0][i] += c[1][i]+c[2][i]+c[3][i];
  RETN;
}

#define INC8_32(_i_) { { unsigned u = ux, v = vx; ux = ctou32(ip+_i_+OV+0); vx = ctou32(ip+_i_+OV+ 4); CU32(u,0); CU32(v,4);}\
                       { unsigned u = ux, v = vx; ux = ctou32(ip+_i_+OV+8); vx = ctou32(ip+_i_+OV+12); CU32(u,0); CU32(v,4);}\
}

unsigned hist_8_32(unsigned char *in, unsigned inlen) { 
  bin_t c[8][CSIZE] = {0},i; 

  unsigned char *ip=in; 
  if(inlen >= 64) {
    unsigned ux = ctou32(ip), vx = ctou32(ip+4);
    for(; ip != in+(inlen&~(64-1))-64; ip += 64) { INC8_32(0); INC8_32(16); INC8_32(32); INC8_32(48); __builtin_prefetch(ip+512, 0); }
  }
  while(ip != in+inlen) c[0][*ip++]++; 

  for(i = 0; i < 256; i++)
    c[0][i] += c[1][i]+c[2][i]+c[3][i]+c[4][i]+c[5][i]+c[6][i]+c[7][i];
  RETN;
} 

//---------------------------------
#define INC4_64(_i_) { unsigned u = cp, v = cp>>32; cp = ctou64(ip+ 8+_i_); CU32(u,0); CU32(v,0); }
unsigned hist_4_64(unsigned char *in, unsigned inlen) { 
  bin_t c[4][CSIZE] = {0},i;
  unsigned char *ip = in; 

  if(inlen >= 64) {
    unsigned long long cp = ctou64(in);  
    for(ip = in; ip != in+(inlen&~(64-1))-64;ip+=64) { INC4_64(0);INC4_64(8);INC4_64(16);INC4_64(24);INC4_64(32);INC4_64(40);INC4_64(48);INC4_64(56);  __builtin_prefetch(ip+512, 0); }
  }
  while(ip != in+inlen) 
    c[0][*ip++]++; 

  for(i = 0; i < 256; i++)
    c[0][i] += c[1][i]+c[2][i]+c[3][i];
  RETN;
}

#define INC8_64(_i_) { unsigned u = cp, v = cp>>32; cp = ctou64(ip+ 8+_i_); CU32(u,0); CU32(v,4); }
unsigned hist_8_64(unsigned char *in, unsigned inlen) { 
  bin_t c[8][CSIZE] = {0},i;
  unsigned char *ip = in; 

  if(inlen >= 64) {
    unsigned long long cp = ctou64(in);  
    for(ip = in; ip != in+(inlen&~(64-1))-64;ip+=64) { INC8_64(0);INC8_64(8);INC8_64(16);INC8_64(24);INC8_64(32);INC8_64(40);INC8_64(48);INC8_64(56);  __builtin_prefetch(ip+512, 0); }
  }
  while(ip != in+inlen) 
    c[0][*ip++]++; 

  for(i = 0; i < 256; i++)
    c[0][i] += c[1][i]+c[2][i]+c[3][i]+c[4][i]+c[5][i]+c[6][i]+c[7][i];
  RETN;
}

  #if defined(__SSE4_1__) || defined(__ARM_NEON)
    #ifdef __SSE4_1__
#include <smmintrin.h>
    #else
#include "sse_neon.h"
    #endif
unsigned hist_4_128(unsigned char *in, unsigned inlen) { 
  bin_t c[4][CSIZE]={0},i; 

  unsigned char *ip;
  __m128i cpv = _mm_loadu_si128((__m128i*)in);
  for(ip = in; ip != in+(inlen&~(32-1)); ) { ip+=16;
    __m128i cv=cpv, dv = _mm_loadu_si128((__m128i*)(ip)); ip+=16; cpv = _mm_loadu_si128((__m128i*)(ip)); 
    c[0][_mm_extract_epi8(cv,  0)]++;
    c[1][_mm_extract_epi8(dv,  0)]++;
    c[2][_mm_extract_epi8(cv,  1)]++;
    c[3][_mm_extract_epi8(dv,  1)]++;
    c[0][_mm_extract_epi8(cv,  2)]++;
    c[1][_mm_extract_epi8(dv,  2)]++;
    c[2][_mm_extract_epi8(cv,  3)]++;
    c[3][_mm_extract_epi8(dv,  3)]++;
    c[0][_mm_extract_epi8(cv,  4)]++;
    c[1][_mm_extract_epi8(dv,  4)]++;
    c[2][_mm_extract_epi8(cv,  5)]++;
    c[3][_mm_extract_epi8(dv,  5)]++;
    c[0][_mm_extract_epi8(cv,  6)]++;
    c[1][_mm_extract_epi8(dv,  6)]++;
    c[2][_mm_extract_epi8(cv,  7)]++;
    c[3][_mm_extract_epi8(dv,  7)]++;
    c[0][_mm_extract_epi8(cv,  8)]++;
    c[1][_mm_extract_epi8(dv,  8)]++;
    c[2][_mm_extract_epi8(cv,  9)]++;
    c[3][_mm_extract_epi8(dv,  9)]++;
    c[0][_mm_extract_epi8(cv, 10)]++;
    c[1][_mm_extract_epi8(dv, 10)]++;
    c[2][_mm_extract_epi8(cv, 11)]++;
    c[3][_mm_extract_epi8(dv, 11)]++;
    c[0][_mm_extract_epi8(cv, 12)]++;
    c[1][_mm_extract_epi8(dv, 12)]++;
    c[2][_mm_extract_epi8(cv, 13)]++;
    c[3][_mm_extract_epi8(dv, 13)]++;
    c[0][_mm_extract_epi8(cv, 14)]++;
    c[1][_mm_extract_epi8(dv, 14)]++;
    c[2][_mm_extract_epi8(cv, 15)]++;
    c[3][_mm_extract_epi8(dv, 15)]++;            PREFETCH(ip+512, 0);
  }
  while(ip < in+inlen) c[0][*ip++]++; 

  for(i = 0; i < 256; i++) 
    c[0][i] += c[1][i]+c[2][i]+c[3][i];
  RETN;
}

unsigned hist_8_128(unsigned char *in, unsigned inlen) { 
  bin_t c[8][CSIZE]={0},i; 

  unsigned char *ip;
  __m128i cpv = _mm_loadu_si128((__m128i*)in);
  for(ip = in; ip != in+(inlen&~(32-1)); ) { ip+=16;
    __m128i cv=cpv, dv = _mm_loadu_si128((__m128i*)(ip)); ip+=16; cpv = _mm_loadu_si128((__m128i*)(ip)); 
    c[0][_mm_extract_epi8(cv,  0)]++;
    c[1][_mm_extract_epi8(dv,  0)]++;
    c[2][_mm_extract_epi8(cv,  1)]++;
    c[3][_mm_extract_epi8(dv,  1)]++;
    c[4][_mm_extract_epi8(cv,  2)]++;
    c[5][_mm_extract_epi8(dv,  2)]++;
    c[6][_mm_extract_epi8(cv,  3)]++;
    c[7][_mm_extract_epi8(dv,  3)]++;
    c[0][_mm_extract_epi8(cv,  4)]++;
    c[1][_mm_extract_epi8(dv,  4)]++;
    c[2][_mm_extract_epi8(cv,  5)]++;
    c[3][_mm_extract_epi8(dv,  5)]++;
    c[4][_mm_extract_epi8(cv,  6)]++;
    c[5][_mm_extract_epi8(dv,  6)]++;
    c[6][_mm_extract_epi8(cv,  7)]++;
    c[7][_mm_extract_epi8(dv,  7)]++;
    c[0][_mm_extract_epi8(cv,  8)]++;
    c[1][_mm_extract_epi8(dv,  8)]++;
    c[2][_mm_extract_epi8(cv,  9)]++;
    c[3][_mm_extract_epi8(dv,  9)]++;
    c[4][_mm_extract_epi8(cv, 10)]++;
    c[5][_mm_extract_epi8(dv, 10)]++;
    c[6][_mm_extract_epi8(cv, 11)]++;
    c[7][_mm_extract_epi8(dv, 11)]++;
    c[0][_mm_extract_epi8(cv, 12)]++;
    c[1][_mm_extract_epi8(dv, 12)]++;
    c[2][_mm_extract_epi8(cv, 13)]++;
    c[3][_mm_extract_epi8(dv, 13)]++;
    c[4][_mm_extract_epi8(cv, 14)]++;
    c[5][_mm_extract_epi8(dv, 14)]++;
    c[6][_mm_extract_epi8(cv, 15)]++;
    c[7][_mm_extract_epi8(dv, 15)]++;            PREFETCH(ip+512, 0);
  }
  while(ip < in+inlen) c[0][*ip++]++; 

  for(i = 0; i < 256; i++) 
    c[0][i] += c[1][i]+c[2][i]+c[3][i]+c[4][i]+c[5][i]+c[6][i]+c[7][i];
  RETN;
}
  #endif

  #ifdef __AVX2__
#include <immintrin.h>
unsigned hist_4_256(unsigned char *in, unsigned inlen) { 
  bin_t c[4][CSIZE]={0},i; 

  unsigned char *ip;
  __m256i cpv = _mm256_loadu_si256((__m256i*)in);
  for(ip = in; ip != in+(inlen&~(32-1)); ) {
    __m256i cv=cpv; cpv = _mm256_loadu_si256((__m256i*)(ip+=32)); 
    c[0][_mm256_extract_epi8(cv,  0)]++;
    c[1][_mm256_extract_epi8(cv,  1)]++;
    c[2][_mm256_extract_epi8(cv,  2)]++;
    c[3][_mm256_extract_epi8(cv,  3)]++;
    c[0][_mm256_extract_epi8(cv,  4)]++;
    c[1][_mm256_extract_epi8(cv,  5)]++;
    c[2][_mm256_extract_epi8(cv,  6)]++;
    c[3][_mm256_extract_epi8(cv,  7)]++;
    c[0][_mm256_extract_epi8(cv,  8)]++;
    c[1][_mm256_extract_epi8(cv,  9)]++;
    c[2][_mm256_extract_epi8(cv, 10)]++;
    c[3][_mm256_extract_epi8(cv, 11)]++;
    c[0][_mm256_extract_epi8(cv, 12)]++;
    c[1][_mm256_extract_epi8(cv, 13)]++;
    c[2][_mm256_extract_epi8(cv, 14)]++;
    c[3][_mm256_extract_epi8(cv, 15)]++;
    c[0][_mm256_extract_epi8(cv, 16)]++;
    c[1][_mm256_extract_epi8(cv, 17)]++;
    c[2][_mm256_extract_epi8(cv, 18)]++;
    c[3][_mm256_extract_epi8(cv, 19)]++;
    c[0][_mm256_extract_epi8(cv, 20)]++;
    c[1][_mm256_extract_epi8(cv, 21)]++;
    c[2][_mm256_extract_epi8(cv, 22)]++;
    c[3][_mm256_extract_epi8(cv, 23)]++;
    c[0][_mm256_extract_epi8(cv, 24)]++;
    c[1][_mm256_extract_epi8(cv, 25)]++;
    c[2][_mm256_extract_epi8(cv, 26)]++;
    c[3][_mm256_extract_epi8(cv, 27)]++;
    c[0][_mm256_extract_epi8(cv, 28)]++;
    c[1][_mm256_extract_epi8(cv, 29)]++;
    c[2][_mm256_extract_epi8(cv, 30)]++;
    c[3][_mm256_extract_epi8(cv, 31)]++;
  }
  while(ip < in+inlen) c[0][*ip++]++; 

  for(i = 0; i < 256; i++) 
    c[0][i] += c[1][i]+c[2][i]+c[3][i];
  RETN;
}

unsigned hist_8_256(unsigned char *in, unsigned inlen) { 
  bin_t c[8][CSIZE]={0},i; 

  unsigned char *ip;
  __m256i cpv = _mm256_loadu_si256((__m256i*)in);
  for(ip = in; ip != in+(inlen&~(32-1)); ) {
    __m256i cv=cpv; cpv = _mm256_loadu_si256((__m256i*)(ip+=32)); 
    c[0][_mm256_extract_epi8(cv,  0)]++;
    c[1][_mm256_extract_epi8(cv,  1)]++;
    c[2][_mm256_extract_epi8(cv,  2)]++;
    c[3][_mm256_extract_epi8(cv,  3)]++;
    c[4][_mm256_extract_epi8(cv,  4)]++;
    c[5][_mm256_extract_epi8(cv,  5)]++;
    c[6][_mm256_extract_epi8(cv,  6)]++;
    c[7][_mm256_extract_epi8(cv,  7)]++;
    c[0][_mm256_extract_epi8(cv,  8)]++;
    c[1][_mm256_extract_epi8(cv,  9)]++;
    c[2][_mm256_extract_epi8(cv, 10)]++;
    c[3][_mm256_extract_epi8(cv, 11)]++;
    c[4][_mm256_extract_epi8(cv, 12)]++;
    c[5][_mm256_extract_epi8(cv, 13)]++;
    c[6][_mm256_extract_epi8(cv, 14)]++;
    c[7][_mm256_extract_epi8(cv, 15)]++;
    c[0][_mm256_extract_epi8(cv, 16)]++;
    c[1][_mm256_extract_epi8(cv, 17)]++;
    c[2][_mm256_extract_epi8(cv, 18)]++;
    c[3][_mm256_extract_epi8(cv, 19)]++;
    c[4][_mm256_extract_epi8(cv, 20)]++;
    c[5][_mm256_extract_epi8(cv, 21)]++;
    c[6][_mm256_extract_epi8(cv, 22)]++;
    c[7][_mm256_extract_epi8(cv, 23)]++;
    c[0][_mm256_extract_epi8(cv, 24)]++;
    c[1][_mm256_extract_epi8(cv, 25)]++;
    c[2][_mm256_extract_epi8(cv, 26)]++;
    c[3][_mm256_extract_epi8(cv, 27)]++;
    c[4][_mm256_extract_epi8(cv, 28)]++;
    c[5][_mm256_extract_epi8(cv, 29)]++;
    c[6][_mm256_extract_epi8(cv, 30)]++;
    c[7][_mm256_extract_epi8(cv, 31)]++;
  }
  while(ip < in+inlen) c[0][*ip++]++; 

  for(i = 0; i < 256; i++) 
    c[0][i] += c[1][i]+c[2][i]+c[3][i]+c[4][i]+c[5][i]+c[6][i]+c[7][i];
  RETN;
}
  #endif

//-------------------------------------------------------------------------
  #ifdef COUNTBENCH
// Not portable "count2x64", fastest function in https://github.com/nkurz/countbench
#define COUNT_SIZE (256+8)
#define ASM_SHIFT_RIGHT(reg, bitsToShift)                                \
    __asm volatile ("shr %1, %0":                                       \
                    "+r" (reg): /* read and written */                  \
                    "i" (bitsToShift) /* constant */                    \
                    )


#define ASM_INC_TABLES(src0, src1, byte0, byte1, offset, size, base, scale) \
    __asm volatile ("movzbl %b2, %k0\n"                /* byte0 = src0 & 0xFF */ \
                    "movzbl %b3, %k1\n"                /* byte1 = src1 & 0xFF */ \
                    "incl (%c4+0)*%c5(%6, %0, %c7)\n"  /* count[i+0][byte0]++ */ \
                    "incl (%c4+1)*%c5(%6, %1, %c7)\n"  /* count[i+1][byte1]++ */ \
                    "movzbl %h2, %k0\n"                /* byte0 = (src0 & 0xFF00) >> 8 */ \
                    "movzbl %h3, %k1\n"                /* byte1 = (src1 & 0xFF00) >> 8 */ \
                    "incl (%c4+2)*%c5(%6, %0, %c7)\n"  /* count[i+2][byte0]++ */ \
                    "incl (%c4+3)*%c5(%6, %1, %c7)\n": /* count[i+3][byte1]++ */ \
                    "=&R" (byte0),  /* write only (R == non REX) */     \
                    "=&R" (byte1):  /* write only (R == non REX) */     \
                    "Q" (src0),  /* read only (Q == must have rH) */    \
                    "Q" (src1),  /* read only (Q == must have rH) */    \
                    "i" (offset), /* constant array offset */           \
                    "i" (size), /* constant array size     */           \
                    "r" (base),  /* read only array address */          \
                    "i" (scale):  /* constant [1,2,4,8] */              \
                    "memory" /* clobbered (forces compiler to compute sum ) */ \
                    )

unsigned count2x64(unsigned char *src, unsigned srcSize)
{
    unsigned long long remainder = srcSize;
    if (srcSize < 32) goto handle_remainder;

    unsigned c[16][COUNT_SIZE];
    memset(c, 0, sizeof(c));
    
    remainder = srcSize % 16;
    srcSize -= remainder;  
    const unsigned char *endSrc = src + srcSize;
    unsigned long long next0 = *(unsigned long long *)(src + 0);
    unsigned long long next1 = *(unsigned long long *)(src + 8);

    //IACA_START;

    while (src != endSrc)
    {
        unsigned long long byte0, byte1;
        unsigned long long data0 = next0;
        unsigned long long data1 = next1;

        src += 16;
        next0 = *(unsigned long long *)(src + 0);
        next1 = *(unsigned long long *)(src + 8);

        ASM_INC_TABLES(data0, data1, byte0, byte1, 0, COUNT_SIZE * 4, c, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 4, COUNT_SIZE * 4, c, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 8, COUNT_SIZE * 4, c, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 12, COUNT_SIZE * 4, c, 4);
    }

    //IACA_END;

 handle_remainder:
    for (size_t i = 0; i < remainder; i++) {
        unsigned long long byte = src[i];
        c[0][byte]++;
    }

    for (int i = 0; i < 256; i++) {
        for (int idx=1; idx < 16; idx++) {
            c[0][i] += c[idx][i];
        }
    }

    RETN;
}

// Modified version of count2x64 by powturbo, using C instead of assembler
#define C_SHIFT_RIGHT(reg, bitsToShift) reg >>= bitsToShift
#define C_INC_TABLES(src0, src1, byte0, byte1, offset, size, c, scale) \
        { \
            byte0 = (unsigned char)src0;\
            byte1 = (unsigned char)src1;\
            c[offset+0][byte0]++;\
            c[offset+1][byte1]++;\
            byte0 = (unsigned char)(src0 >> 8);\
            byte1 = (unsigned char)(src1 >> 8);\
            c[offset+2][byte0]++; \
            c[offset+3][byte1]++; \
        }

unsigned count2x64c(unsigned char *src, unsigned srcSize)
{
    unsigned long long remainder = srcSize;
    if (srcSize < 32) goto handle_remainder;

    unsigned c[16][COUNT_SIZE];
    memset(c, 0, sizeof(c));
    
    remainder = srcSize % 16;
    srcSize -= remainder;  
    const unsigned char *endSrc = src + srcSize;
    unsigned long long next0 = *(unsigned long long *)(src + 0);
    unsigned long long next1 = *(unsigned long long *)(src + 8);

    //IACA_START;

    while (src != endSrc)
    {
        unsigned long long byte0, byte1;
        unsigned long long data0 = next0;
        unsigned long long data1 = next1;

        src += 16;
        next0 = *(unsigned long long *)(src + 0);
        next1 = *(unsigned long long *)(src + 8);

        C_INC_TABLES(data0, data1, byte0, byte1, 0, COUNT_SIZE * 4, c, 4);

        C_SHIFT_RIGHT(data0, 16);
        C_SHIFT_RIGHT(data1, 16);
        C_INC_TABLES(data0, data1, byte0, byte1, 4, COUNT_SIZE * 4, c, 4);

        C_SHIFT_RIGHT(data0, 16);
        C_SHIFT_RIGHT(data1, 16);
        C_INC_TABLES(data0, data1, byte0, byte1, 8, COUNT_SIZE * 4, c, 4);

        C_SHIFT_RIGHT(data0, 16);
        C_SHIFT_RIGHT(data1, 16);
        C_INC_TABLES(data0, data1, byte0, byte1, 12, COUNT_SIZE * 4, c, 4);
    }

    //IACA_END;

 handle_remainder:
    for (size_t i = 0; i < remainder; i++) {
        unsigned long long byte = src[i];
        c[0][byte]++;
    }

    for (int i = 0; i < 256; i++) {
        for (int idx=1; idx < 16; idx++) {
            c[0][i] += c[idx][i];
        }
    }

    RETN;
}
  #endif

