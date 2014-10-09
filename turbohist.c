// compile w. gcc -O3 -march=native turbohist.c -o turbohist
// homepage: http://sites.google.com/site/powturbo/ 
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

  #ifdef __SSE4_1__
#include <smmintrin.h>
  #endif

#define CHKSIZE (1<<30) // (56*1024) //
//------------------------------------------------------------------------------------
//#define RET { unsigned a = 256; while(a > 1 && !c0[a-1]) a--; return a;}    		// alphabet maximum
//#define RET { unsigned a=0, i; for(i = 0; i < 256; i++) a+=c0[i]; return a;}  	// Count
//#define RET { unsigned a=0, i; for(i = 0; i < 256; i++) a+=i*c0[i]; return a;}  	// CheckSum
#define RET return c0[0]  								// benchmark

  #if CHKSIZE <= (1<<16)
typedef unsigned short bin_t;
#define PAD8 0
  #else
typedef unsigned       bin_t;
#define PAD8 8  // +8 to avoid repeated call into the same cache line pool (cache associativity). by Nathan Kurz : https://github.com/powturbo/turbohist/issues/2
  #endif

int hist_1_8(unsigned char *in, unsigned inlen) { 
  bin_t c0[256] = {0};

  unsigned char *ip;
  for(ip=in; ip != in+inlen;)
    c0[*ip++]++;
  RET;
}

int hist_4_8(unsigned char *in, unsigned inlen) { 
  bin_t c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0}; 

  unsigned char *ip; 
  for(ip = in; ip != in + (inlen&~(4-1)); ) 
    c0[*ip++]++, c1[*ip++]++, c2[*ip++]++, c3[*ip++]++; 
  while(ip < in+inlen) 
    c0[*ip++]++; 

  int i;for(i = 0; i < 256; i++) c0[i] += c1[i]+c2[i]+c3[i];
  RET;
}

int hist_8_8(unsigned char *in, unsigned inlen) { 
  bin_t c0[256+PAD8]={0},c1[256+PAD8]={0},c2[256+PAD8]={0},c3[256+PAD8]={0},c4[256+PAD8]={0},c5[256+PAD8]={0},c6[256+PAD8]={0},c7[256+PAD8]={0}; 

  unsigned char *ip; 
  for(ip=in; ip != in + (inlen&~(8-1)); ) 
    c0[*ip++]++, c1[*ip++]++, c2[*ip++]++, c3[*ip++]++, c4[*ip++]++, c5[*ip++]++, c6[*ip++]++, c7[*ip++]++; 
  while(ip < in+inlen) 
    c0[*ip++]++; 

  int i;for(i = 0; i < 256; i++) c0[i] += c1[i]+c2[i]+c3[i]+c4[i]+c5[i]+c6[i]+c7[i];
  RET;
}

int hist_4_32(unsigned char *in, unsigned inlen) { 
  bin_t c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0}; 

  unsigned char *ip;
  unsigned 		         cp = *(unsigned *)in;
  for(ip = in; ip != in+(inlen&~(16-1));) {
    unsigned 	c = cp; ip += 4; cp = *(unsigned *)ip;
    unsigned 	d = cp; ip += 4; cp = *(unsigned *)ip;
    c0[(unsigned char) c     ]++;
    c1[(unsigned char) d     ]++;
    c2[(unsigned char)(c>> 8)]++;
    c3[(unsigned char)(d>> 8)]++;
    c0[(unsigned char)(c>>16)]++;
    c1[(unsigned char)(d>>16)]++;
    c2[(unsigned char)(c>>24)]++;
    c3[                d>>24]++;
    
    		c = cp; ip += 4; cp = *(unsigned *)ip;
    		d = cp; ip += 4; cp = *(unsigned *)ip;
    c0[(unsigned char)c      ]++;
    c1[(unsigned char)d      ]++;
    c2[(unsigned char)(c>>8) ]++;
    c3[(unsigned char)(d>>8) ]++;
    c0[(unsigned char)(c>>16)]++;
    c1[(unsigned char)(d>>16)]++;
    c2[(unsigned char)(c>>24)]++;
    c3[                d>>24]++;
  }
  while(ip < in+inlen) c0[*ip++]++; 

  int i;
  for(i = 0; i < 256; i++) 
    c0[i] += c1[i]+c2[i]+c3[i];
  RET;
}

int hist_8_32(unsigned char *in, unsigned inlen) { 
  bin_t c0[256+PAD8]={0},c1[256+PAD8]={0},c2[256+PAD8]={0},c3[256+PAD8]={0},c4[256+PAD8]={0},c5[256+PAD8]={0},c6[256+PAD8]={0},c7[256+PAD8]={0}; 

  unsigned char *ip;
  unsigned 		       cp = *(unsigned *)in;
  for(ip = in; ip != in+(inlen&~(16-1));) { 
    unsigned 	c = cp; ip += 4; cp = *(unsigned *)ip;
    unsigned 	d = cp; ip += 4; cp = *(unsigned *)ip;
    c0[(unsigned char) c     ]++;
    c1[(unsigned char) d     ]++;
    c2[(unsigned char)(c>> 8)]++;
    c3[(unsigned char)(d>> 8)]++;
    c4[(unsigned char)(c>>16)]++;
    c5[(unsigned char)(d>>16)]++;
    c6[                c>>24 ]++;
    c7[                d>>24 ]++;
    
    		c = cp; ip += 4; cp = *(unsigned *)ip;
    		d = cp; ip += 4; cp = *(unsigned *)ip;
    c0[(unsigned char) c     ]++;
    c1[(unsigned char) d     ]++;
    c2[(unsigned char)(c>> 8)]++;
    c3[(unsigned char)(d>> 8)]++;
    c4[(unsigned char)(c>>16)]++;
    c5[(unsigned char)(d>>16)]++;
    c6[                c>>24 ]++;
    c7[                d>>24 ]++;    
  }
  while(ip < in+inlen) c0[*ip++]++; 

  int i;
  for(i = 0; i < 256; i++) 
    c0[i] += c1[i]+c2[i]+c3[i]+c4[i]+c5[i]+c6[i]+c7[i];
  RET;
}

int hist_4_64(unsigned char *in, unsigned inlen) { 
  bin_t c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0}; 

  unsigned char *ip;
  unsigned long long 			cp = *(unsigned long long *)in;
  for(ip = in; ip != in+(inlen&~(16-1)); ) {    
    unsigned long long c = cp; ip += 8; cp = *(unsigned long long *)ip; 
    unsigned long long d = cp; ip += 8; cp = *(unsigned long long *)ip; 
    c0[(unsigned char) c     ]++;
    c1[(unsigned char)(c>>8) ]++;
    c2[(unsigned char)(c>>16)]++;
    c3[(unsigned char)(c>>24)]++;
    c0[(unsigned char)(c>>32)]++;
    c1[(unsigned char)(c>>40)]++;
    c2[(unsigned char)(c>>48)]++;
    c3[                c>>56 ]++;

    c0[(unsigned char) d     ]++;
    c1[(unsigned char)(d>>8) ]++;
    c2[(unsigned char)(d>>16)]++;
    c3[(unsigned char)(d>>24)]++;
    c0[(unsigned char)(d>>32)]++;
    c1[(unsigned char)(d>>40)]++;
    c2[(unsigned char)(d>>48)]++;
    c3[                d>>56 ]++;
  }
  while(ip < in+inlen) c0[*ip++]++;
 
  int i;
  for(i = 0; i < 256; i++) 
    c0[i] += c1[i]+c2[i]+c3[i];
  RET;
}

int hist_8_64(unsigned char *in, unsigned inlen) { 
  bin_t c0[256+PAD8]={0},c1[256+PAD8]={0},c2[256+PAD8]={0},c3[256+PAD8]={0},c4[256+PAD8]={0},c5[256+PAD8]={0},c6[256+PAD8]={0},c7[256+PAD8]={0}; 
  unsigned char *ip;

  unsigned long long 			cp = *(unsigned long long *)in;
  for(ip = in; ip != in+(inlen&~(16-1)); ) {    
    unsigned long long c = cp; ip += 8; cp = *(unsigned long long *)ip; 
    unsigned long long d = cp; ip += 8; cp = *(unsigned long long *)ip; 
    c0[(unsigned char) c     ]++;
    c1[(unsigned char)(c>>8) ]++;
    c2[(unsigned char)(c>>16)]++;
    c3[(unsigned char)(c>>24)]++;
    c4[(unsigned char)(c>>32)]++;
    c5[(unsigned char)(c>>40)]++;
    c6[(unsigned char)(c>>48)]++;
    c7[                c>>56 ]++;

    c0[(unsigned char) d     ]++;
    c1[(unsigned char)(d>>8) ]++;
    c2[(unsigned char)(d>>16)]++;
    c3[(unsigned char)(d>>24)]++;
    c4[(unsigned char)(d>>32)]++;
    c5[(unsigned char)(d>>40)]++;
    c6[(unsigned char)(d>>48)]++;
    c7[                d>>56 ]++;
  }
  while(ip < in+inlen) c0[*ip++]++; 

  int i;
  for(i = 0; i < 256; i++) 
    c0[i] += c1[i]+c2[i]+c3[i]+c4[i]+c5[i]+c6[i]+c7[i];
  RET;
}

  #ifdef __SSE4_1__
int hist_4_128(unsigned char *in, unsigned inlen) { 
  bin_t c0[256+PAD8]={0},c1[256+PAD8]={0},c2[256+PAD8]={0},c3[256+PAD8]={0}; 

  unsigned char *ip;
  __m128i cpv = _mm_loadu_si128((__m128i*)in);
  for(ip = in; ip != in+(inlen&~(32-1)); ) {
    __m128i cv=cpv; 	    ip += 16; cpv = _mm_loadu_si128((__m128i*)ip); 
    __m128i dv=cpv;         ip += 16; cpv = _mm_loadu_si128((__m128i*)ip); 
    c0[_mm_extract_epi8(cv,  0)]++;
    c1[_mm_extract_epi8(cv,  1)]++;
    c2[_mm_extract_epi8(cv,  2)]++;
    c3[_mm_extract_epi8(cv,  3)]++;
    c0[_mm_extract_epi8(cv,  4)]++;
    c1[_mm_extract_epi8(cv,  5)]++;
    c2[_mm_extract_epi8(cv,  6)]++;
    c3[_mm_extract_epi8(cv,  7)]++;
    c0[_mm_extract_epi8(cv,  8)]++;
    c1[_mm_extract_epi8(cv,  9)]++;
    c2[_mm_extract_epi8(cv, 10)]++;
    c3[_mm_extract_epi8(cv, 11)]++;
    c0[_mm_extract_epi8(cv, 12)]++;
    c1[_mm_extract_epi8(cv, 13)]++;
    c2[_mm_extract_epi8(cv, 14)]++;
    c3[_mm_extract_epi8(cv, 15)]++;

    c0[_mm_extract_epi8(dv,  0)]++;
    c1[_mm_extract_epi8(dv,  1)]++;
    c2[_mm_extract_epi8(dv,  2)]++;
    c3[_mm_extract_epi8(dv,  3)]++;
    c0[_mm_extract_epi8(dv,  4)]++;
    c1[_mm_extract_epi8(dv,  5)]++;
    c2[_mm_extract_epi8(dv,  6)]++;
    c3[_mm_extract_epi8(dv,  7)]++;
    c0[_mm_extract_epi8(dv,  8)]++;
    c1[_mm_extract_epi8(dv,  9)]++;
    c2[_mm_extract_epi8(dv, 10)]++;
    c3[_mm_extract_epi8(dv, 11)]++;
    c0[_mm_extract_epi8(dv, 12)]++;
    c1[_mm_extract_epi8(dv, 13)]++;
    c2[_mm_extract_epi8(dv, 14)]++;
    c3[_mm_extract_epi8(dv, 15)]++;
  }
  while(ip < in+inlen) c0[*ip++]++; 

  int i;
  for(i = 0; i < 256; i++) 
    c0[i] += c1[i]+c2[i]+c3[i];
  RET;
}

int hist_8_128(unsigned char *in, unsigned inlen) { 
  bin_t c0[256+PAD8]={0},c1[256+PAD8]={0},c2[256+PAD8]={0},c3[256+PAD8]={0},c4[256+PAD8]={0},c5[256+PAD8]={0},c6[256+PAD8]={0},c7[256+PAD8]={0}; 

  unsigned char *ip;
  __m128i cpv = _mm_loadu_si128((__m128i*)in);
  for(ip = in; ip != in+(inlen&~(32-1)); ) {
    __m128i cv=cpv; 	    ip += 16; cpv = _mm_loadu_si128((__m128i*)ip); 
    __m128i dv=cpv;         ip += 16; cpv = _mm_loadu_si128((__m128i*)ip); 
    c0[_mm_extract_epi8(cv,  0)]++;
    c1[_mm_extract_epi8(cv,  1)]++;
    c2[_mm_extract_epi8(cv,  2)]++;
    c3[_mm_extract_epi8(cv,  3)]++;
    c4[_mm_extract_epi8(cv,  4)]++;
    c5[_mm_extract_epi8(cv,  5)]++;
    c6[_mm_extract_epi8(cv,  6)]++;
    c7[_mm_extract_epi8(cv,  7)]++;
    c0[_mm_extract_epi8(cv,  8)]++;
    c1[_mm_extract_epi8(cv,  9)]++;
    c2[_mm_extract_epi8(cv, 10)]++;
    c3[_mm_extract_epi8(cv, 11)]++;
    c4[_mm_extract_epi8(cv, 12)]++;
    c5[_mm_extract_epi8(cv, 13)]++;
    c6[_mm_extract_epi8(cv, 14)]++;
    c7[_mm_extract_epi8(cv, 15)]++;

    c0[_mm_extract_epi8(dv,  0)]++;
    c1[_mm_extract_epi8(dv,  1)]++;
    c2[_mm_extract_epi8(dv,  2)]++;
    c3[_mm_extract_epi8(dv,  3)]++;
    c4[_mm_extract_epi8(dv,  4)]++;
    c5[_mm_extract_epi8(dv,  5)]++;
    c6[_mm_extract_epi8(dv,  6)]++;
    c7[_mm_extract_epi8(dv,  7)]++;
    c0[_mm_extract_epi8(dv,  8)]++;
    c1[_mm_extract_epi8(dv,  9)]++;
    c2[_mm_extract_epi8(dv, 10)]++;
    c3[_mm_extract_epi8(dv, 11)]++;
    c4[_mm_extract_epi8(dv, 12)]++;
    c5[_mm_extract_epi8(dv, 13)]++;
    c6[_mm_extract_epi8(dv, 14)]++;
    c7[_mm_extract_epi8(dv, 15)]++;
  }
  while(ip < in+inlen) c0[*ip++]++; 

  int i;
  for(i = 0; i < 256; i++) 
    c0[i] += c1[i]+c2[i]+c3[i]+c4[i]+c5[i]+c6[i]+c7[i];
  RET;
}
  #endif
//--------------------------------------------------------------------------------------
//#define NKURTZ
  #ifdef NKURTZ
// fastest, but not portable "count2x64" from https://github.com/nkurz/countbench
#include <stdint.h>
#define COUNT_SIZE (256 + 8)

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

int count2x64(uint8_t *src, size_t srcSize)
{
    uint64_t remainder = srcSize;
    if (srcSize < 32) goto handle_remainder;

    uint32_t count[16][COUNT_SIZE];
    memset(count, 0, sizeof(count));
    
    remainder = srcSize % 16;
    srcSize -= remainder;  
    const uint8_t *endSrc = src + srcSize;
    uint64_t next0 = *(uint64_t *)(src + 0);
    uint64_t next1 = *(uint64_t *)(src + 8);

    //IACA_START;

    while (src != endSrc)
    {
        uint64_t byte0, byte1;
        uint64_t data0 = next0;
        uint64_t data1 = next1;

        src += 16;
        next0 = *(uint64_t *)(src + 0);
        next1 = *(uint64_t *)(src + 8);

        ASM_INC_TABLES(data0, data1, byte0, byte1, 0, COUNT_SIZE * 4, count, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 4, COUNT_SIZE * 4, count, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 8, COUNT_SIZE * 4, count, 4);

        ASM_SHIFT_RIGHT(data0, 16);
        ASM_SHIFT_RIGHT(data1, 16);
        ASM_INC_TABLES(data0, data1, byte0, byte1, 12, COUNT_SIZE * 4, count, 4);
    }

    //IACA_END;
 size_t i;
 handle_remainder:
    for (i = 0; i < remainder; i++) {
        uint64_t byte = src[i];
        count[0][byte]++;
    }

    for (i = 0; i < 256; i++) { int idx;
        for (idx=1; idx < 16; idx++) {
            count[0][i] += count[idx][i];
        }
    }

    return count[0][0];
}
  #endif

//----------------------------------------------------------------------------------------------------------------------------
typedef unsigned long long tm_t;
static tm_t tmtime(void);

  #ifdef _MSC_VER
#include <intrin.h>
  #else
#include <x86intrin.h>
  #endif

  #ifdef _WIN32
#include <windows.h>
#define TM_T 1

static tm_t tmtime(void) {
  LARGE_INTEGER tm;
  QueryPerformanceCounter(&tm);
  return (tm_t)(tm.QuadPart/tps.QuadPart);
}

LARGE_INTEGER tps;
static tm_t tminit() { QueryPerformanceFrequency(&tps); tm_t t0=tmtime(),ts; while((ts = tmtime())==t0); return ts; } 
  #else
#include <sys/time.h>
#include <sys/resource.h>
#define TM_T 1000000.0
static tm_t tmtime(void) {
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return (tm_t)tm.tv_sec*1000000ull + tm.tv_usec;
}

static tm_t tminit() {                                  tm_t t0=tmtime(),ts; while((ts = tmtime())==t0); return ts; }
  #endif

//----------------------------------------------------------------------------------
#define MBS 1000000.0 //MiBS 1048576.0

#define TMPRINT(__x) { printf("%7.2f MB/s\t%3.1f clocks/symbol\tc=%d it=%d\t%s\n", (double)(tc>=0.000001?(((double)n*it/MBS)/(((double)tc/1)/TM_T)):0.0), (double)cc/((double)n*it), r, it, __x); }
#define TMBEG for(t0=tminit(),c0=__rdtsc(),it=0;it<reps;) { r = 0;
#define TMEND it++; if((tc = tmtime() - t0) > tx) break; } cc=__rdtsc()-c0

  #if CHKSIZE <= (1<<16)
#define process(__fct, __in, __inlen) { unsigned char *ip = __in, *in_=__in+__inlen; int iplen;\
  for(; ip < in_; ip += iplen) { iplen = in_ - ip; if(iplen > CHKSIZE) iplen = CHKSIZE;\
    r += __fct(ip, iplen); \
  }\
}
  #else
#define process(__fct, __in, __inlen) r = __fct(__in, __inlen)
  #endif

//---------------------------------------------------------------------------------------------------
#define GB (1<<30)

int main(int argc, char *argv[]) { int i; unsigned char *in;int n,reps=GB; tm_t tx=1*1000000; 
  { char *inname = argv[1];  

    FILE *fi = fopen(inname,  "rb"); 
    if(!fi ) perror(inname), exit(1);  
    printf("'%s'\n", inname);

    fseek(fi, 0, SEEK_END); 
    long long flen = ftell(fi); 
    fseek(fi, 0, SEEK_SET);

    if(flen > GB) flen = GB;
    n = flen; 
    if(!(in = (unsigned char*)malloc(n))) { fprintf(stderr, "malloc error\n"); exit(-1); }
    n = fread(in, 1, n, fi);
    fclose(fi);
    if(n <= 0) exit(0); 
  }

  { 
    int r,k,it;
    tm_t t0,tc,c0,cc;
    TMBEG process(hist_4_32, in,n);	TMEND;	//warmup
    TMBEG process(hist_8_32, in,n);	TMEND;	TMPRINT("hist_8_32");
    TMBEG process(hist_4_32, in,n);	TMEND;	TMPRINT("hist_4_32");

      #ifdef NKURTZ
    TMBEG process(count2x64, in,n); 	TMEND;	TMPRINT("count2x64 nkurz");
      #endif 

      #ifdef __SSE4_1__
    TMBEG process(hist_8_128,in,n);  	TMEND;	TMPRINT("hist_8_128");
    TMBEG process(hist_4_128,in,n);  	TMEND;	TMPRINT("hist_4_128");
      #endif
    TMBEG process(hist_8_64, in,n); 	TMEND;	TMPRINT("hist_8_64");
    TMBEG process(hist_4_64, in,n); 	TMEND;	TMPRINT("hist_4_64");

    TMBEG process(hist_8_8,  in,n); 	TMEND;	TMPRINT("hist_8_8");
    TMBEG process(hist_4_8,  in,n); 	TMEND;	TMPRINT("hist_4_8");
    TMBEG process(hist_1_8,  in,n); 	TMEND;	TMPRINT("hist_1_8");
  }
  free(in);
}
