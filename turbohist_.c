/**
Copyright (c) 2013-2022, Powturbo
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
// 1D Histogram: hist[r]_X_Y   r:run aware  X: number bins used,   Y: processing unit   1:8 bits, 4:32 bits, 8:64 bits 
#include "conf.h"
  #ifdef __ARM_NEON
#define PREFETCH(_ip_,_rw_)
  #else
#define PREFETCH(_ip_,_rw_) __builtin_prefetch(_ip_,_rw_)
  #endif
  
#define CSIZE (256 + 8)
typedef unsigned cnt_t;

  #if 1 // fast when auto-vectorization enabled (for ex. with gcc -O3)
#define HISTEND(_c_,_cn_,_cnt_) { int _i,_j;\
  memset(_cnt_, 0, 256*sizeof(_cnt_[0]));\
  for(_i=0; _i < 256; _i++)\
    for(_j=0; _j < _cn_;_j++) _cnt_[_i] += _c_[_j][_i];\
}	

#define HISTEND8(_c_,_cnt_) HISTEND(_c_,8,_cnt_)
#define HISTEND4(_c_,_cnt_) HISTEND(_c_,4,_cnt_)
  #else
static ALWAYS_INLINE histend4(cnt_t c[4][CSIZE], cnt_t *__restrict cnt) { unsigned i;
  #ifdef __AVX2__
  for(i = 0; i != 256; i+=8) {
    __m256i sv =                  _mm256_load_si256((const __m256i *)&c[0][i]);
            sv = _mm256_add_epi32(_mm256_load_si256((const __m256i *)&c[1][i]), sv);
            sv = _mm256_add_epi32(_mm256_load_si256((const __m256i *)&c[2][i]), sv);
            sv = _mm256_add_epi32(_mm256_load_si256((const __m256i *)&c[3][i]), sv);
    _mm256_storeu_si256((__m256i *)&cnt[i], sv);
  }
  #elif defined(__SSE2__) || defined(__ARM_NEON)
  for(i = 0; i != 256; i+=4) {
    __m128i sv =                  _mm_load_si128((const __m128i *)&c[0][i]);
            sv = _mm256_add_epi32(_mm_load_si128((const __m128i *)&c[1][i]), sv);
            sv = _mm256_add_epi32(_mm_load_si128((const __m128i *)&c[2][i]), sv);
            sv = _mm256_add_epi32(_mm_load_si128((const __m128i *)&c[3][i]), sv);
    _mm_storeu_si128((__m128i *)&cnt[i], sv);
  }
    #else
  for(i = 0; i != 256; i++) cnt[i] = c[0][i]+c[1][i]+c[2][i]+c[3][i];
	#endif
}

static ALWAYS_INLINE histend8(cnt_t c[8][CSIZE], cnt_t *__restrict cnt) { unsigned i;
    #ifdef __AVX2__ 
  for(i = 0; i != 256; i+=8) {
    __m256i v0 = _mm256_load_si256((const __m256i *)&c[0][i]);
    __m256i v1 = _mm256_load_si256((const __m256i *)&c[1][i]);
	__m256i s0 = _mm256_add_epi32(v0, v1);
            v0 = _mm256_load_si256((const __m256i *)&c[2][i]);
            v1 = _mm256_load_si256((const __m256i *)&c[3][i]);
	__m256i s1 = _mm256_add_epi32(v0, v1);
            s0 = _mm256_add_epi32(s0, s1);
			
            v0 = _mm256_load_si256((const __m256i *)&c[4][i]);
            v1 = _mm256_load_si256((const __m256i *)&c[5][i]);
			s1 = _mm256_add_epi32(v0, v1);
            v0 = _mm256_load_si256((const __m256i *)&c[6][i]);
            v1 = _mm256_load_si256((const __m256i *)&c[7][i]);
	        s0 = _mm256_add_epi32(s0, v0);	
			s1 = _mm256_add_epi32(s1, v1);
			
    _mm256_storeu_si256((__m256i *)&cnt[i], _mm256_add_epi32(s0, s1));
  }
    #elif defined(__SSE2__) || defined(__ARM_NEON)
  for(i = 0; i != 256; i+=4) {
    __m128i v0 = _mm_load_si128((const __m128i *)&c[0][i]);
    __m128i v1 = _mm_load_si128((const __m128i *)&c[1][i]);
	__m128i sv = _mm_add_epi32(v0, v1);
            v0 = _mm_load_si128((const __m128i *)&c[2][i]);
            v1 = _mm_load_si128((const __m128i *)&c[3][i]);
	        sv = _mm_add_epi32(sv, v0);
	        sv = _mm_add_epi32(sv, v1);

            v0 = _mm_load_si128((const __m128i *)&c[4][i]);
            v1 = _mm_load_si128((const __m128i *)&c[5][i]);
	        sv = _mm_add_epi32(sv, v0);
	        sv = _mm_add_epi32(sv, v1);
            v0 = _mm_load_si128((const __m128i *)&c[6][i]);
            v1 = _mm_load_si128((const __m128i *)&c[7][i]);
	        sv = _mm_add_epi32(sv, v0);
    _mm_storeu_si128((__m128i *)&cnt[i], _mm_add_epi32(sv, v1));
  }
    #else
  for(i = 0; i != 256; i++) cnt[i] = c[0][i]+c[1][i]+c[2][i]+c[3][i]+c[4][i]+c[5][i]+c[6][i]+c[7][i];
	#endif
}

#define HISTEND8(_c_,_cnt_) histend8(_c_,_cnt_)
#define HISTEND4(_c_,_cnt_) histend4(_c_,_cnt_)
  #endif

//---------------------------- 8 bits ------------------------------------------------------
static void hist_1_8(unsigned char *__restrict in, unsigned inlen, cnt_t *__restrict cnt) {
  unsigned char *ip = in;
  
  memset(cnt, 0, 256*sizeof(cnt[0]));
  while(ip < in+inlen) cnt[*ip++]++;
}

static void hist_4_8(unsigned char *__restrict in, unsigned inlen, cnt_t *__restrict cnt) { 
  cnt_t c[4][CSIZE] = {0},i;
  unsigned char *ip = in; 
  
  while(ip != in+(inlen&~(4-1))) c[0][*ip++]++, c[1][*ip++]++, c[2][*ip++]++, c[3][*ip++]++;
  while(ip != in+ inlen        ) c[0][*ip++]++;
  HISTEND4(c, cnt);
}

static void hist_8_8(unsigned char *__restrict in, unsigned inlen, cnt_t *__restrict cnt) { 
  cnt_t c[8][CSIZE] = {0},i; 
  unsigned char *ip = in; 
  
  while(ip != in+(inlen&~(8-1))) c[0][*ip++]++, c[1][*ip++]++, c[2][*ip++]++, c[3][*ip++]++, c[4][*ip++]++, c[5][*ip++]++, c[6][*ip++]++, c[7][*ip++]++; 
  while(ip != in+ inlen        ) c[0][*ip++]++; 
  HISTEND8(c, cnt);
}

//----------------------------- 32 bits --------------------------------------------------------
  #if defined(__i386__) || defined(__x86_64__) 
#define CU32(_u_,_i_,_c_) {\
  c[_i_+0][(unsigned char )(_u_)   ]+=_c_;\
  c[_i_+1][(unsigned short)(_u_)>>8]+=_c_; _u_>>=16;\
  c[_i_+2][(unsigned char )(_u_)   ]+=_c_;\
  c[_i_+3][(unsigned short)(_u_)>>8]+=_c_;\
}
  #else
#define CU32(_u_,_i_,_c_) {\
  c[_i_+0][(unsigned char) (_u_)     ]+=_c_;\
  c[_i_+1][(unsigned char)((_u_)>> 8)]+=_c_;\
  c[_i_+2][(unsigned char)((_u_)>>16)]+=_c_;\
  c[_i_+3][ 	           (_u_)>>24 ]+=_c_;\
}
  #endif

#define UZ 4 // Load size 1x 32 bits = 4 bytes
#define I132(_i_,_o_) { unsigned u1 = ctou32(ip+UZ+_i_*UZ*2+0); CU32(u0, 0, 1);\
                                 u0 = ctou32(ip+UZ+_i_*UZ*2+4); CU32(u1,_o_,1);\
}

#define N32 32
static void hist_4_32(unsigned char *__restrict in, unsigned inlen, cnt_t *__restrict cnt) {
  #define IC 0
  cnt_t    c[4][CSIZE] = {0}, i; 
  unsigned char *ip = in; 
  
  if(inlen >= UZ+N32) {
    unsigned u0 = ctou32(ip);
    for(; ip <= in+inlen-(UZ+N32); ip += N32) { 
	  I132(0,IC); I132(1,IC); I132(2,IC); I132(3,IC);  			
														PREFETCH(ip+512, 0); 
	}
  }
  while(ip != in+inlen) c[0][*ip++]++; 
  HISTEND4(c, cnt);
}

static void hist_8_32(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) { 
  #define IC 4
  cnt_t c[8][CSIZE] = {0}, i; 
  unsigned char *ip = in; 
  
  if(inlen >= UZ+N32) {
    unsigned u0 = ctou32(ip);
    for(; ip <= in+inlen-(UZ+N32); ip += N32) { 
	  I132(0,IC); I132(1,IC); I132(2,IC); I132(3,IC);	  //I132(4,IC); I132(5,IC); I132(6,IC); I132(7,IC);			
	  PREFETCH(ip+512, 0); 
	}
  }
  while(ip != in+inlen) c[0][*ip++]++; 
  HISTEND8(c, cnt);
} 

//--------------------  64 bits ---------------------------------------------------
  #if defined(__i386__) || defined(__x86_64__) 
#define CU64(_u_,_o_,_c_) { unsigned _x = _u_;\
  c[0    ][(unsigned char )_x    ]+=_c_;\
  c[1    ][(unsigned short)_x>> 8]+=_c_; _x>>=16;\
  c[2    ][(unsigned char )_x    ]+=_c_;\
  c[3    ][(unsigned short)_x>> 8]+=_c_; _x=(_u_)>>=32;\
  c[0+_o_][(unsigned char )_x    ]+=_c_;\
  c[1+_o_][(unsigned short)_x>> 8]+=_c_; _x>>=16;\
  c[2+_o_][(unsigned char )_x    ]+=_c_;\
  c[3+_o_][(unsigned short)_x>> 8]+=_c_;\
}
  #else
#define CU64(_u_,_o_,_c_) { unsigned _x = _u_;\
  c[0    ][(unsigned char) _x     ]+=_c_;\
  c[1    ][(unsigned char)(_x>> 8)]+=_c_;\
  c[2    ][(unsigned char)(_x>>16)]+=_c_;\
  c[3    ][                _x>>24 ]+=_c_;  _x=(_u_)>>=32;\
  c[0+_o_][(unsigned char) _x     ]+=_c_;\
  c[1+_o_][(unsigned char)(_x>> 8)]+=_c_;\
  c[2+_o_][(unsigned char)(_x>>16)]+=_c_;\
  c[3+_o_][                _x>>24 ]+=_c_;\
}
  #endif

#define UZ 8 // Load size 1x 64 bits = 8 bytes
#define I164(_i_,_o_) { uint64_t u1 = ctou64(ip+UZ+_i_*UZ*2+ 0); CU64(u0, _o_, 1);\
                                 u0 = ctou64(ip+UZ+_i_*UZ*2+ 8); CU64(u1, _o_, 1);\
}

#define N64 64
static void hist_4_64(unsigned char *__restrict in, unsigned inlen, cnt_t *__restrict cnt) {
  #define IC 0
  cnt_t         c[4][CSIZE] = {0}, i; 
  unsigned char *ip = in; 
  
  if(inlen >= UZ+N64) {
    uint64_t u0 = ctou64(ip);
    for(; ip <= in+inlen-(UZ+N64); ip += N64) { 
	  I164(0,IC); I164(1,IC); I164(2,IC); I164(3,IC);
																	PREFETCH(ip+512, 0); 
	}
  }
  while(ip != in+inlen) c[0][*ip++]++; 
  HISTEND4(c, cnt);
}
   
static void hist_8_64(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) {
  #define IC 4
  cnt_t c[8][CSIZE] = {0}, i; 
  unsigned char *ip = in; 
  
  if(inlen >= UZ+N64) {
    uint64_t u0 = ctou64(ip);
    for(; ip <= in+inlen-(UZ+N64); ip += N64) { 
	  I164(0,IC); I164(1,IC); I164(2,IC); I164(3,IC);
																	PREFETCH(ip+512, 0); 
	}
  } 
  while(ip != in+inlen) c[0][*ip++]++; 
  HISTEND8(c, cnt);
}

//-----  hist_8_64a with inline assembly -----------------------------------------
  #ifdef __x86_64
#define RSHR(r, b) __asm volatile ("shr %1, %0": "+r" (r): "i" (b) )
	
#define CU16(x, u,  offset, size, base, scale) \
  __asm volatile (\
    "movzbl %b1, %k0\n"\
    "incl (%c2+0)*%c3(%4, %0, %c5)\n"\
    "movzbl %h1, %k0\n"\
    "incl (%c2+1)*%c3(%4, %0, %c5)\n"\
    :"=&R" (x)\
    :"Q" (u), "i" (offset), "i" (size), "r" (base), "i" (scale) \
    :"memory"\
  ) 
				  
#define N64 64					
unsigned hist_8_64a(unsigned char *in, unsigned inlen, unsigned *__restrict cnt) {
  unsigned      c[8][CSIZE]= {0};    
  unsigned char *ip = in; 
  
  if(inlen >= 8+N64) {
    uint64_t u0 = ctou64(ip),b;
    for(; ip <= in+inlen-(8+N64); ip += N64) { 
      uint64_t x, u1;
	  #define ST(u)  CU16(x, u, 0, CSIZE*4, c, 4);\
	    RSHR(u, 16); CU16(x, u, 2, CSIZE*4, c, 4);\
        RSHR(u, 16); CU16(x, u, 4, CSIZE*4, c, 4);\
        RSHR(u, 16); CU16(x, u, 6, CSIZE*4, c, 4); 
	  u1 = ctou64(ip+8+ 0); ST(u0);
      u0 = ctou64(ip+8+ 8); ST(u1);
      u1 = ctou64(ip+8+16); ST(u0);
      u0 = ctou64(ip+8+24); ST(u1);
      u1 = ctou64(ip+8+32); ST(u0);
      u0 = ctou64(ip+8+40); ST(u1);
      u1 = ctou64(ip+8+48); ST(u0);
      u0 = ctou64(ip+8+56); ST(u1);				PREFETCH(ip+768, 0);
    }
  }
  while(ip < in+inlen) c[0][*ip++]++; 
  HISTEND8(c, cnt);
}
#endif

#define UZ 16 // Load size 2x 64 bits = 2*8 bytes
#define CR64(u,v,_o_,_c_) if(likely(u!=v)) { CU64(u,_o_,1); CU64(v,_o_,1); } else if((u^(v<<8)) < (1<<8)) c[_c_][(unsigned char)u]+=UZ; else CU64(u, _o_,2)
#define I2R64(_i_,_o_) { uint64_t u1 = ctou64(ip+UZ+_i_*UZ*2+ 0), v1 = ctou64(ip+UZ+_i_*UZ*2+ 8); CR64(u0,v0,_o_,_i_);\
                                  u0 = ctou64(ip+UZ+_i_*UZ*2+16); v0 = ctou64(ip+UZ+_i_*UZ*2+24); CR64(u1,v1,_o_,_i_);\
}

#define N64 64
static void histr_4_64(unsigned char *__restrict in, unsigned inlen, cnt_t *__restrict cnt) {
  #define IC 0
  cnt_t c[4][CSIZE] = {0},i; 
  unsigned char *ip = in,*in_; 
  
  if(inlen >= UZ+N64) {
    uint64_t u0 = ctou64(ip), v0 = ctou64(ip+8);
    for(; ip <= in+inlen-(UZ+N64); ip += N64) { 
	  I2R64(0,IC); I2R64(1,IC); 
																PREFETCH(ip+512, 0); 
	}
  }
  while(ip != in+inlen)
    c[0][*ip++]++; 
  HISTEND4(c, cnt);
}

static void histr_8_64(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) {
  #define IC 4
  cnt_t c[8][CSIZE] = {0},i; 
  unsigned char *ip = in,*in_; 
  
  if(inlen >= UZ+N64) {
    uint64_t u0 = ctou64(ip), v0 = ctou64(ip+8);
    for(; ip <= in+inlen-(UZ+N64); ip += N64) { 
	  I2R64(0,IC); I2R64(1,IC); 
																PREFETCH(ip+512, 0); 
	}
  }
  while(ip != in+inlen) c[0][*ip++]++; 
  HISTEND8(c, cnt);
}

  #if defined(__SSE4_1__) || defined(__ARM_NEON) //---------- sse4.1 ---------------------------------------
    #ifdef __SSE4_1__
#include <smmintrin.h>
    #else
#include "sse_neon.h"
    #endif
static void hist_4_128(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) {
  cnt_t c[4][CSIZE]={0},i; 

  unsigned char *ip = in;
  if(inlen >= 32+64) {
      __m128i u0 = _mm_loadu_si128((__m128i*)ip), v0 = _mm_loadu_si128((__m128i*)(ip+16));
    for(; ip <= in+inlen-(32+64); ip += 64) { 
      __m128i u1 = _mm_loadu_si128((__m128i*)(ip+32)), v1 = _mm_loadu_si128((__m128i*)(ip+32+16)); 
      c[0][_mm_extract_epi8(u0,  0)]++;
      c[1][_mm_extract_epi8(v0,  0)]++;
      c[2][_mm_extract_epi8(u0,  1)]++;
      c[3][_mm_extract_epi8(v0,  1)]++;
      c[0][_mm_extract_epi8(u0,  2)]++;
      c[1][_mm_extract_epi8(v0,  2)]++;
      c[2][_mm_extract_epi8(u0,  3)]++;
      c[3][_mm_extract_epi8(v0,  3)]++;
      c[0][_mm_extract_epi8(u0,  4)]++;
      c[1][_mm_extract_epi8(v0,  4)]++;
      c[2][_mm_extract_epi8(u0,  5)]++;
      c[3][_mm_extract_epi8(v0,  5)]++;
      c[0][_mm_extract_epi8(u0,  6)]++;
      c[1][_mm_extract_epi8(v0,  6)]++;
      c[2][_mm_extract_epi8(u0,  7)]++;
      c[3][_mm_extract_epi8(v0,  7)]++;
      c[0][_mm_extract_epi8(u0,  8)]++;
      c[1][_mm_extract_epi8(v0,  8)]++;
      c[2][_mm_extract_epi8(u0,  9)]++;
      c[3][_mm_extract_epi8(v0,  9)]++;
      c[0][_mm_extract_epi8(u0, 10)]++;
      c[1][_mm_extract_epi8(v0, 10)]++;
      c[2][_mm_extract_epi8(u0, 11)]++;
      c[3][_mm_extract_epi8(v0, 11)]++;
      c[0][_mm_extract_epi8(u0, 12)]++;
      c[1][_mm_extract_epi8(v0, 12)]++;
      c[2][_mm_extract_epi8(u0, 13)]++;
      c[3][_mm_extract_epi8(v0, 13)]++;
      c[0][_mm_extract_epi8(u0, 14)]++;
      c[1][_mm_extract_epi8(v0, 14)]++;
      c[2][_mm_extract_epi8(u0, 15)]++;
      c[3][_mm_extract_epi8(v0, 15)]++;
	
	  u0 = _mm_loadu_si128((__m128i*)(ip+32+32)); v0 = _mm_loadu_si128((__m128i*)(ip+32+48));
      c[0][_mm_extract_epi8(u1,  0)]++;
      c[1][_mm_extract_epi8(v1,  0)]++;
      c[2][_mm_extract_epi8(u1,  1)]++;
      c[3][_mm_extract_epi8(v1,  1)]++;
      c[0][_mm_extract_epi8(u1,  2)]++;
      c[1][_mm_extract_epi8(v1,  2)]++;
      c[2][_mm_extract_epi8(u1,  3)]++;
      c[3][_mm_extract_epi8(v1,  3)]++;
      c[0][_mm_extract_epi8(u1,  4)]++;
      c[1][_mm_extract_epi8(v1,  4)]++;
      c[2][_mm_extract_epi8(u1,  5)]++;
      c[3][_mm_extract_epi8(v1,  5)]++;
      c[0][_mm_extract_epi8(u1,  6)]++;
      c[1][_mm_extract_epi8(v1,  6)]++;
      c[2][_mm_extract_epi8(u1,  7)]++;
      c[3][_mm_extract_epi8(v1,  7)]++;
      c[0][_mm_extract_epi8(u1,  8)]++;
      c[1][_mm_extract_epi8(v1,  8)]++;
      c[2][_mm_extract_epi8(u1,  9)]++;
      c[3][_mm_extract_epi8(v1,  9)]++;
      c[0][_mm_extract_epi8(u1, 10)]++;
      c[1][_mm_extract_epi8(v1, 10)]++;
      c[2][_mm_extract_epi8(u1, 11)]++;
      c[3][_mm_extract_epi8(v1, 11)]++;
      c[0][_mm_extract_epi8(u1, 12)]++;
      c[1][_mm_extract_epi8(v1, 12)]++;
      c[2][_mm_extract_epi8(u1, 13)]++;
      c[3][_mm_extract_epi8(v1, 13)]++;
      c[0][_mm_extract_epi8(u1, 14)]++;
      c[1][_mm_extract_epi8(v1, 14)]++;
      c[2][_mm_extract_epi8(u1, 15)]++;
      c[3][_mm_extract_epi8(v1, 15)]++;										PREFETCH(ip+512, 0);
    }
  }
  while(ip < in+inlen) c[0][*ip++]++; 
  HISTEND4(c, cnt);
}

unsigned hist_8_128(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) { 
  cnt_t c[8][CSIZE]={0},i; 

  unsigned char *ip = in;
  if(inlen >= 32+64) {
      __m128i u0 = _mm_loadu_si128((__m128i*)ip), v0 = _mm_loadu_si128((__m128i*)(ip+16));
    for(; ip <= in+inlen-(32+64); ip += 64) { 
      __m128i u1 = _mm_loadu_si128((__m128i*)(ip+32)), v1 = _mm_loadu_si128((__m128i*)(ip+32+16)); 
      c[0][_mm_extract_epi8(u0,  0)]++;
      c[1][_mm_extract_epi8(v0,  0)]++;
      c[2][_mm_extract_epi8(u0,  1)]++;
      c[3][_mm_extract_epi8(v0,  1)]++;
      c[4][_mm_extract_epi8(u0,  2)]++;
      c[5][_mm_extract_epi8(v0,  2)]++;
      c[6][_mm_extract_epi8(u0,  3)]++;
      c[7][_mm_extract_epi8(v0,  3)]++;
      c[0][_mm_extract_epi8(u0,  4)]++;
      c[1][_mm_extract_epi8(v0,  4)]++;
      c[2][_mm_extract_epi8(u0,  5)]++;
      c[3][_mm_extract_epi8(v0,  5)]++;
      c[4][_mm_extract_epi8(u0,  6)]++;
      c[5][_mm_extract_epi8(v0,  6)]++;
      c[6][_mm_extract_epi8(u0,  7)]++;
      c[7][_mm_extract_epi8(v0,  7)]++;
      c[0][_mm_extract_epi8(u0,  8)]++;
      c[1][_mm_extract_epi8(v0,  8)]++;
      c[2][_mm_extract_epi8(u0,  9)]++;
      c[3][_mm_extract_epi8(v0,  9)]++;
      c[4][_mm_extract_epi8(u0, 10)]++;
      c[5][_mm_extract_epi8(v0, 10)]++;
      c[6][_mm_extract_epi8(u0, 11)]++;
      c[7][_mm_extract_epi8(v0, 11)]++;
      c[0][_mm_extract_epi8(u0, 12)]++;
      c[1][_mm_extract_epi8(v0, 12)]++;
      c[2][_mm_extract_epi8(u0, 13)]++;
      c[3][_mm_extract_epi8(v0, 13)]++;
      c[4][_mm_extract_epi8(u0, 14)]++;
      c[5][_mm_extract_epi8(v0, 14)]++;
      c[6][_mm_extract_epi8(u0, 15)]++;
      c[7][_mm_extract_epi8(v0, 15)]++; 
	
	  u0 = _mm_loadu_si128((__m128i*)(ip+32+32)); v0 = _mm_loadu_si128((__m128i*)(ip+32+48));
      c[0][_mm_extract_epi8(u1,  0)]++;
      c[1][_mm_extract_epi8(v1,  0)]++;
      c[2][_mm_extract_epi8(u1,  1)]++;
      c[3][_mm_extract_epi8(v1,  1)]++;
      c[4][_mm_extract_epi8(u1,  2)]++;
      c[5][_mm_extract_epi8(v1,  2)]++;
      c[6][_mm_extract_epi8(u1,  3)]++;
      c[7][_mm_extract_epi8(v1,  3)]++;
      c[0][_mm_extract_epi8(u1,  4)]++;
      c[1][_mm_extract_epi8(v1,  4)]++;
      c[2][_mm_extract_epi8(u1,  5)]++;
      c[3][_mm_extract_epi8(v1,  5)]++;
      c[4][_mm_extract_epi8(u1,  6)]++;
      c[5][_mm_extract_epi8(v1,  6)]++;
      c[6][_mm_extract_epi8(u1,  7)]++;
      c[7][_mm_extract_epi8(v1,  7)]++;
      c[0][_mm_extract_epi8(u1,  8)]++;
      c[1][_mm_extract_epi8(v1,  8)]++;
      c[2][_mm_extract_epi8(u1,  9)]++;
      c[3][_mm_extract_epi8(v1,  9)]++;
      c[4][_mm_extract_epi8(u1, 10)]++;
      c[5][_mm_extract_epi8(v1, 10)]++;
      c[6][_mm_extract_epi8(u1, 11)]++;
      c[7][_mm_extract_epi8(v1, 11)]++;
      c[0][_mm_extract_epi8(u1, 12)]++;
      c[1][_mm_extract_epi8(v1, 12)]++;
      c[2][_mm_extract_epi8(u1, 13)]++;
      c[3][_mm_extract_epi8(v1, 13)]++;
      c[4][_mm_extract_epi8(u1, 14)]++;
      c[5][_mm_extract_epi8(v1, 14)]++;
      c[6][_mm_extract_epi8(u1, 15)]++;
      c[7][_mm_extract_epi8(v1, 15)]++;          	  							PREFETCH(ip+512, 0);
    }
  }
  while(ip < in+inlen) c[0][*ip++]++; 
  HISTEND8(c, cnt);
}
  #endif

  #ifdef __AVX2__ //---------------------------------- avx2 -----------------------------------------------
#include <immintrin.h>

#define UZ   64
#define N256 128

static void hist_4_256(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) { 
  cnt_t c[4][CSIZE]={0},i; 

  unsigned char *ip = in;
  if(inlen >= UZ+N256) {
      __m256i u0 = _mm256_loadu_si256((__m256i*)ip),      v0 = _mm256_loadu_si256((__m256i*)(ip+32));
    for(; ip <= in+((inlen-(UZ+N256))&~(N256-1)); ip += N256) { 
      __m256i u1 = _mm256_loadu_si256((__m256i*)(ip+UZ)), v1 = _mm256_loadu_si256((__m256i*)(ip+UZ+32)); 	
      c[0][_mm256_extract_epi8(u0,  0)]++;
      c[1][_mm256_extract_epi8(v0,  0)]++;
      c[2][_mm256_extract_epi8(u0,  1)]++;
      c[3][_mm256_extract_epi8(v0,  1)]++;
      c[0][_mm256_extract_epi8(u0,  2)]++;
      c[1][_mm256_extract_epi8(v0,  2)]++;
      c[2][_mm256_extract_epi8(u0,  3)]++;
      c[3][_mm256_extract_epi8(v0,  3)]++;
      c[0][_mm256_extract_epi8(u0,  4)]++;
      c[1][_mm256_extract_epi8(v0,  4)]++;
      c[2][_mm256_extract_epi8(u0,  5)]++;
      c[3][_mm256_extract_epi8(v0,  5)]++;
      c[0][_mm256_extract_epi8(u0,  6)]++;
      c[1][_mm256_extract_epi8(v0,  6)]++;
      c[2][_mm256_extract_epi8(u0,  7)]++;
      c[3][_mm256_extract_epi8(v0,  7)]++;
      c[0][_mm256_extract_epi8(u0,  8)]++;
      c[1][_mm256_extract_epi8(v0,  8)]++;
      c[2][_mm256_extract_epi8(u0,  9)]++;
      c[3][_mm256_extract_epi8(v0,  9)]++;
      c[0][_mm256_extract_epi8(u0, 10)]++;
      c[1][_mm256_extract_epi8(v0, 10)]++;
      c[2][_mm256_extract_epi8(u0, 11)]++;
      c[3][_mm256_extract_epi8(v0, 11)]++;
      c[0][_mm256_extract_epi8(u0, 12)]++;
      c[1][_mm256_extract_epi8(v0, 12)]++;
      c[2][_mm256_extract_epi8(u0, 13)]++;
      c[3][_mm256_extract_epi8(v0, 13)]++;
      c[0][_mm256_extract_epi8(u0, 14)]++;
      c[1][_mm256_extract_epi8(v0, 14)]++;
      c[2][_mm256_extract_epi8(u0, 15)]++;
      c[3][_mm256_extract_epi8(v0, 15)]++;
      c[0][_mm256_extract_epi8(u0, 16)]++;
      c[1][_mm256_extract_epi8(v0, 16)]++;
      c[2][_mm256_extract_epi8(u0, 17)]++;
      c[3][_mm256_extract_epi8(v0, 17)]++;
      c[0][_mm256_extract_epi8(u0, 18)]++;
      c[1][_mm256_extract_epi8(v0, 18)]++;
      c[2][_mm256_extract_epi8(u0, 19)]++;
      c[3][_mm256_extract_epi8(v0, 19)]++;
      c[0][_mm256_extract_epi8(u0, 20)]++;
      c[1][_mm256_extract_epi8(v0, 20)]++;
      c[2][_mm256_extract_epi8(u0, 21)]++;
      c[3][_mm256_extract_epi8(v0, 21)]++;
      c[0][_mm256_extract_epi8(u0, 22)]++;
      c[1][_mm256_extract_epi8(v0, 22)]++;
      c[2][_mm256_extract_epi8(u0, 23)]++;
      c[3][_mm256_extract_epi8(v0, 23)]++;
      c[0][_mm256_extract_epi8(u0, 24)]++;
      c[1][_mm256_extract_epi8(v0, 24)]++;
      c[2][_mm256_extract_epi8(u0, 25)]++;
      c[3][_mm256_extract_epi8(v0, 25)]++;
      c[0][_mm256_extract_epi8(u0, 26)]++;
      c[1][_mm256_extract_epi8(v0, 26)]++;
      c[2][_mm256_extract_epi8(u0, 27)]++;
      c[3][_mm256_extract_epi8(v0, 27)]++;
      c[0][_mm256_extract_epi8(u0, 28)]++;
      c[1][_mm256_extract_epi8(v0, 28)]++;
      c[2][_mm256_extract_epi8(u0, 29)]++;
      c[3][_mm256_extract_epi8(v0, 29)]++;
      c[0][_mm256_extract_epi8(u0, 30)]++;
      c[1][_mm256_extract_epi8(v0, 30)]++;
      c[2][_mm256_extract_epi8(u0, 31)]++;
      c[3][_mm256_extract_epi8(v0, 31)]++;

      u0 = _mm256_loadu_si256((__m256i*)(ip+UZ+64)); v0 = _mm256_loadu_si256((__m256i*)(ip+UZ+96)); 	
      c[0][_mm256_extract_epi8(u1,  0)]++;
      c[1][_mm256_extract_epi8(v1,  0)]++;
      c[2][_mm256_extract_epi8(u1,  1)]++;
      c[3][_mm256_extract_epi8(v1,  1)]++;
      c[0][_mm256_extract_epi8(u1,  2)]++;
      c[1][_mm256_extract_epi8(v1,  2)]++;
      c[2][_mm256_extract_epi8(u1,  3)]++;
      c[3][_mm256_extract_epi8(v1,  3)]++;
      c[0][_mm256_extract_epi8(u1,  4)]++;
      c[1][_mm256_extract_epi8(v1,  4)]++;
      c[2][_mm256_extract_epi8(u1,  5)]++;
      c[3][_mm256_extract_epi8(v1,  5)]++;
      c[0][_mm256_extract_epi8(u1,  6)]++;
      c[1][_mm256_extract_epi8(v1,  6)]++;
      c[2][_mm256_extract_epi8(u1,  7)]++;
      c[3][_mm256_extract_epi8(v1,  7)]++;
      c[0][_mm256_extract_epi8(u1,  8)]++;
      c[1][_mm256_extract_epi8(v1,  8)]++;
      c[2][_mm256_extract_epi8(u1,  9)]++;
      c[3][_mm256_extract_epi8(v1,  9)]++;
      c[0][_mm256_extract_epi8(u1, 10)]++;
      c[1][_mm256_extract_epi8(v1, 10)]++;
      c[2][_mm256_extract_epi8(u1, 11)]++;
      c[3][_mm256_extract_epi8(v1, 11)]++;
      c[0][_mm256_extract_epi8(u1, 12)]++;
      c[1][_mm256_extract_epi8(v1, 12)]++;
      c[2][_mm256_extract_epi8(u1, 13)]++;
      c[3][_mm256_extract_epi8(v1, 13)]++;
      c[0][_mm256_extract_epi8(u1, 14)]++;
      c[1][_mm256_extract_epi8(v1, 14)]++;
      c[2][_mm256_extract_epi8(u1, 15)]++;
      c[3][_mm256_extract_epi8(v1, 15)]++;
      c[0][_mm256_extract_epi8(u1, 16)]++;
      c[1][_mm256_extract_epi8(v1, 16)]++;
      c[2][_mm256_extract_epi8(u1, 17)]++;
      c[3][_mm256_extract_epi8(v1, 17)]++;
      c[0][_mm256_extract_epi8(u1, 18)]++;
      c[1][_mm256_extract_epi8(v1, 18)]++;
      c[2][_mm256_extract_epi8(u1, 19)]++;
      c[3][_mm256_extract_epi8(v1, 19)]++;
      c[0][_mm256_extract_epi8(u1, 20)]++;
      c[1][_mm256_extract_epi8(v1, 20)]++;
      c[2][_mm256_extract_epi8(u1, 21)]++;
      c[3][_mm256_extract_epi8(v1, 21)]++;
      c[0][_mm256_extract_epi8(u1, 22)]++;
      c[1][_mm256_extract_epi8(v1, 22)]++;
      c[2][_mm256_extract_epi8(u1, 23)]++;
      c[3][_mm256_extract_epi8(v1, 23)]++;
      c[0][_mm256_extract_epi8(u1, 24)]++;
      c[1][_mm256_extract_epi8(v1, 24)]++;
      c[2][_mm256_extract_epi8(u1, 25)]++;
      c[3][_mm256_extract_epi8(v1, 25)]++;
      c[0][_mm256_extract_epi8(u1, 26)]++;
      c[1][_mm256_extract_epi8(v1, 26)]++;
      c[2][_mm256_extract_epi8(u1, 27)]++;
      c[3][_mm256_extract_epi8(v1, 27)]++;
      c[0][_mm256_extract_epi8(u1, 28)]++;
      c[1][_mm256_extract_epi8(v1, 28)]++;
      c[2][_mm256_extract_epi8(u1, 29)]++;
      c[3][_mm256_extract_epi8(v1, 29)]++;
      c[0][_mm256_extract_epi8(u1, 30)]++;
      c[1][_mm256_extract_epi8(v1, 30)]++;
      c[2][_mm256_extract_epi8(u1, 31)]++;
      c[3][_mm256_extract_epi8(v1, 31)]++;							PREFETCH(ip+512, 0);
    }
  }
  while(ip < in+inlen) c[0][*ip++]++; 
  HISTEND4(c, cnt);
}

#define UZ   64
#define N256 128
static void hist_8_256(unsigned char *__restrict in, unsigned inlen, unsigned *__restrict cnt) { 
  cnt_t c[8][CSIZE]={0},i; 

  unsigned char *ip = in;
  if(inlen >= UZ+N256) {
      __m256i u0 = _mm256_loadu_si256((__m256i*)ip),        v0 = _mm256_loadu_si256((__m256i*)(ip+32));
    for(; ip <= in+((inlen-(UZ+N256))&~(N256-1)); ip += N256) { 
      __m256i u1 = _mm256_loadu_si256((__m256i*)(ip+UZ+0)), v1 = _mm256_loadu_si256((__m256i*)(ip+UZ+32)); 	
      c[0][_mm256_extract_epi8(u0,  0)]++;
      c[1][_mm256_extract_epi8(v0,  0)]++;
      c[2][_mm256_extract_epi8(u0,  1)]++;
      c[3][_mm256_extract_epi8(v0,  1)]++;
      c[4][_mm256_extract_epi8(u0,  2)]++;
      c[5][_mm256_extract_epi8(v0,  2)]++;
      c[6][_mm256_extract_epi8(u0,  3)]++;
      c[7][_mm256_extract_epi8(v0,  3)]++;
      c[0][_mm256_extract_epi8(u0,  4)]++;
      c[1][_mm256_extract_epi8(v0,  4)]++;
      c[2][_mm256_extract_epi8(u0,  5)]++;
      c[3][_mm256_extract_epi8(v0,  5)]++;
      c[4][_mm256_extract_epi8(u0,  6)]++;
      c[5][_mm256_extract_epi8(v0,  6)]++;
      c[6][_mm256_extract_epi8(u0,  7)]++;
      c[7][_mm256_extract_epi8(v0,  7)]++;
      c[0][_mm256_extract_epi8(u0,  8)]++;
      c[1][_mm256_extract_epi8(v0,  8)]++;
      c[2][_mm256_extract_epi8(u0,  9)]++;
      c[3][_mm256_extract_epi8(v0,  9)]++;
      c[4][_mm256_extract_epi8(u0, 10)]++;
      c[5][_mm256_extract_epi8(v0, 10)]++;
      c[6][_mm256_extract_epi8(u0, 11)]++;
      c[7][_mm256_extract_epi8(v0, 11)]++;
      c[0][_mm256_extract_epi8(u0, 12)]++;
      c[1][_mm256_extract_epi8(v0, 12)]++;
      c[2][_mm256_extract_epi8(u0, 13)]++;
      c[3][_mm256_extract_epi8(v0, 13)]++;
      c[4][_mm256_extract_epi8(u0, 14)]++;
      c[5][_mm256_extract_epi8(v0, 14)]++;
      c[6][_mm256_extract_epi8(u0, 15)]++;
      c[7][_mm256_extract_epi8(v0, 15)]++;
      c[0][_mm256_extract_epi8(u0, 16)]++;
      c[1][_mm256_extract_epi8(v0, 16)]++;
      c[2][_mm256_extract_epi8(u0, 17)]++;
      c[3][_mm256_extract_epi8(v0, 17)]++;
      c[4][_mm256_extract_epi8(u0, 18)]++;
      c[5][_mm256_extract_epi8(v0, 18)]++;
      c[6][_mm256_extract_epi8(u0, 19)]++;
      c[7][_mm256_extract_epi8(v0, 19)]++;
      c[0][_mm256_extract_epi8(u0, 20)]++;
      c[1][_mm256_extract_epi8(v0, 20)]++;
      c[2][_mm256_extract_epi8(u0, 21)]++;
      c[3][_mm256_extract_epi8(v0, 21)]++;
      c[4][_mm256_extract_epi8(u0, 22)]++;
      c[5][_mm256_extract_epi8(v0, 22)]++;
      c[6][_mm256_extract_epi8(u0, 23)]++;
      c[7][_mm256_extract_epi8(v0, 23)]++;
      c[0][_mm256_extract_epi8(u0, 24)]++;
      c[1][_mm256_extract_epi8(v0, 24)]++;
      c[2][_mm256_extract_epi8(u0, 25)]++;
      c[3][_mm256_extract_epi8(v0, 25)]++;
      c[4][_mm256_extract_epi8(u0, 26)]++;
      c[5][_mm256_extract_epi8(v0, 26)]++;
      c[6][_mm256_extract_epi8(u0, 27)]++;
      c[7][_mm256_extract_epi8(v0, 27)]++;
      c[0][_mm256_extract_epi8(u0, 28)]++;
      c[1][_mm256_extract_epi8(v0, 28)]++;
      c[2][_mm256_extract_epi8(u0, 29)]++;
      c[3][_mm256_extract_epi8(v0, 29)]++;
      c[4][_mm256_extract_epi8(u0, 30)]++;
      c[5][_mm256_extract_epi8(v0, 30)]++;
      c[6][_mm256_extract_epi8(u0, 31)]++;
      c[7][_mm256_extract_epi8(v0, 31)]++;

      u0 = _mm256_loadu_si256((__m256i*)(ip+UZ+64)); v0 = _mm256_loadu_si256((__m256i*)(ip+UZ+96)); 	
      c[0][_mm256_extract_epi8(u1,  0)]++;
      c[1][_mm256_extract_epi8(v1,  0)]++;
      c[2][_mm256_extract_epi8(u1,  1)]++;
      c[3][_mm256_extract_epi8(v1,  1)]++;
      c[4][_mm256_extract_epi8(u1,  2)]++;
      c[5][_mm256_extract_epi8(v1,  2)]++;
      c[6][_mm256_extract_epi8(u1,  3)]++;
      c[7][_mm256_extract_epi8(v1,  3)]++;
      c[0][_mm256_extract_epi8(u1,  4)]++;
      c[1][_mm256_extract_epi8(v1,  4)]++;
      c[2][_mm256_extract_epi8(u1,  5)]++;
      c[3][_mm256_extract_epi8(v1,  5)]++;
      c[4][_mm256_extract_epi8(u1,  6)]++;
      c[5][_mm256_extract_epi8(v1,  6)]++;
      c[6][_mm256_extract_epi8(u1,  7)]++;
      c[7][_mm256_extract_epi8(v1,  7)]++;
      c[0][_mm256_extract_epi8(u1,  8)]++;
      c[1][_mm256_extract_epi8(v1,  8)]++;
      c[2][_mm256_extract_epi8(u1,  9)]++;
      c[3][_mm256_extract_epi8(v1,  9)]++;
      c[4][_mm256_extract_epi8(u1, 10)]++;
      c[5][_mm256_extract_epi8(v1, 10)]++;
      c[6][_mm256_extract_epi8(u1, 11)]++;
      c[7][_mm256_extract_epi8(v1, 11)]++;
      c[0][_mm256_extract_epi8(u1, 12)]++;
      c[1][_mm256_extract_epi8(v1, 12)]++;
      c[2][_mm256_extract_epi8(u1, 13)]++;
      c[3][_mm256_extract_epi8(v1, 13)]++;
      c[4][_mm256_extract_epi8(u1, 14)]++;
      c[5][_mm256_extract_epi8(v1, 14)]++;
      c[6][_mm256_extract_epi8(u1, 15)]++;
      c[7][_mm256_extract_epi8(v1, 15)]++;
      c[0][_mm256_extract_epi8(u1, 16)]++;
      c[1][_mm256_extract_epi8(v1, 16)]++;
      c[2][_mm256_extract_epi8(u1, 17)]++;
      c[3][_mm256_extract_epi8(v1, 17)]++;
      c[4][_mm256_extract_epi8(u1, 18)]++;
      c[5][_mm256_extract_epi8(v1, 18)]++;
      c[6][_mm256_extract_epi8(u1, 19)]++;
      c[7][_mm256_extract_epi8(v1, 19)]++;
      c[0][_mm256_extract_epi8(u1, 20)]++;
      c[1][_mm256_extract_epi8(v1, 20)]++;
      c[2][_mm256_extract_epi8(u1, 21)]++;
      c[3][_mm256_extract_epi8(v1, 21)]++;
      c[4][_mm256_extract_epi8(u1, 22)]++;
      c[5][_mm256_extract_epi8(v1, 22)]++;
      c[6][_mm256_extract_epi8(u1, 23)]++;
      c[7][_mm256_extract_epi8(v1, 23)]++;
      c[0][_mm256_extract_epi8(u1, 24)]++;
      c[1][_mm256_extract_epi8(v1, 24)]++;
      c[2][_mm256_extract_epi8(u1, 25)]++;
      c[3][_mm256_extract_epi8(v1, 25)]++;
      c[4][_mm256_extract_epi8(u1, 26)]++;
      c[5][_mm256_extract_epi8(v1, 26)]++;
      c[6][_mm256_extract_epi8(u1, 27)]++;
      c[7][_mm256_extract_epi8(v1, 27)]++;
      c[0][_mm256_extract_epi8(u1, 28)]++;
      c[1][_mm256_extract_epi8(v1, 28)]++;
      c[2][_mm256_extract_epi8(u1, 29)]++;
      c[3][_mm256_extract_epi8(v1, 29)]++;
      c[4][_mm256_extract_epi8(u1, 30)]++;
      c[5][_mm256_extract_epi8(v1, 30)]++;
      c[6][_mm256_extract_epi8(u1, 31)]++;
      c[7][_mm256_extract_epi8(v1, 31)]++;               PREFETCH(ip+512, 0);
    }
  }
  while(ip < in+inlen) c[0][*ip++]++; 
  HISTEND8(c, cnt);
}
  #endif

//-------------------------------------------------------------------------
  #ifdef _COUNTBENCH
// "count2x64", fastest function in https://github.com/nkurz/countbench
#define CSIZE (256+8)
					
#define ASM_SHIFT_RIGHT(reg, bitsToShift)                               \
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
					
unsigned count2x64(unsigned char *src, unsigned srcSize, unsigned *__restrict cnt)
{
    unsigned long long remainder = srcSize;
    if (srcSize < 32) goto handle_remainder;

    unsigned c[16][CSIZE];
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

        ASM_INC_TABLES(data0, data1, byte0, byte1, 0, CSIZE * 4, c, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 4, CSIZE * 4, c, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 8, CSIZE * 4, c, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 12, CSIZE * 4, c, 4);
    }

    //IACA_END;

 handle_remainder:
    for (size_t i = 0; i < remainder; i++) {
        unsigned long long byte = src[i];
        c[0][byte]++;
    }
    memset(cnt, 0, 256*sizeof(cnt[0]));
    for(int i = 0; i < 256; i++)
      for (int idx=0; idx < 16; idx++)
        cnt[i] += c[idx][i];
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

static void count2x64c(unsigned char *__restrict src, unsigned srcSize, unsigned *__restrict cnt)
{
    unsigned long long remainder = srcSize;
    if (srcSize < 32) goto handle_remainder;

    unsigned c[16][CSIZE];
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

        C_INC_TABLES(data0, data1, byte0, byte1, 0, CSIZE * 4, c, 4);

        C_SHIFT_RIGHT(data0, 16);
        C_SHIFT_RIGHT(data1, 16);
        C_INC_TABLES(data0, data1, byte0, byte1, 4, CSIZE * 4, c, 4);

        C_SHIFT_RIGHT(data0, 16);
        C_SHIFT_RIGHT(data1, 16);
        C_INC_TABLES(data0, data1, byte0, byte1, 8, CSIZE * 4, c, 4);

        C_SHIFT_RIGHT(data0, 16);
        C_SHIFT_RIGHT(data1, 16);
        C_INC_TABLES(data0, data1, byte0, byte1, 12, CSIZE * 4, c, 4);
    }

    //IACA_END;

 handle_remainder:
    for (size_t i = 0; i < remainder; i++) {
        unsigned long long byte = src[i];
        c[0][byte]++;
    }
    memset(cnt, 0, 256*sizeof(cnt[0]));
    for(int i = 0; i < 256; i++) 
      for(int idx=0; idx < 16; idx++)
        cnt[i] += c[idx][i];
}
  #endif
