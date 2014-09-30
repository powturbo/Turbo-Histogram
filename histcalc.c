// compile w. gcc -w -O2 -msse4.1 histcalc.c -o histcalc

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

  #ifdef __SSE4_1__
#include <smmintrin.h>
  #endif

//------------------------------------------------------------------------------------
  #ifndef _WIN32
#include <sys/resource.h>
  #endif
typedef unsigned long long tm_t;
static double tmsec(tm_t tm) { return (double)tm/1000000.0; }
static double tmmsec(tm_t tm) { return (double)tm/1000.0; }
static tm_t tmtime(void);

  #ifdef _WIN32
#include <windows.h>
#define TM_T 1
LARGE_INTEGER tps;
static tm_t tminit() {
  QueryPerformanceFrequency(&tps); tm_t t0=tmtime(); while(t0 == tmtime());return t0;
} 
  #else
#define TM_T 1000000.0
static tm_t  tminit() { tm_t t0=tmtime(); while(t0 == tmtime()); return t0;}
  #endif

static tm_t tmtime(void) 
{
    #ifdef _WIN32
  LARGE_INTEGER tm;
  QueryPerformanceCounter(&tm);
  return (tm_t)(tm.QuadPart/tps.QuadPart);
    #else 
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return (tm_t)tm.tv_sec*1000000ull + tm.tv_usec;
    #endif
}
//-------------------------------------------------------------------------------------

//#define RET { unsigned a = 256; while(a > 1 && !count[a-1]) a--; return a;}
#define RET { unsigned a=0, i; for(i = 0; i < 256; i++) a+=count[i]; return a;}

int count_1(unsigned char *in, unsigned inlen) { 
  unsigned count[256] = {0};
  unsigned char *ip;

  for(ip=in; ip != in + inlen;)
    count[*ip++]++;
  RET;
}

int count_4(unsigned char *in, unsigned inlen) { 
  unsigned count[256]={0};
  unsigned c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0}; 
  unsigned char *ip; 

  for(ip = in; ip != in + (inlen&~(4-1)); ) 
    c0[*ip++]++, c1[*ip++]++, c2[*ip++]++, c3[*ip++]++; 
  while(ip < in+inlen) 
    c0[*ip++]++; 
  int i;for(i = 0; i < 256; i++) count[i] = c0[i]+c1[i]+c2[i]+c3[i];

  RET;
}

int count_8(unsigned char *in, unsigned inlen) { 
  unsigned int count[256]={0};
  unsigned c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0},c4[256]={0},c5[256]={0},c6[256]={0},c7[256]={0};  
  unsigned char *ip; 

  for(ip=in; ip != in + (inlen&~(8-1)); ) 
    c0[*ip++]++, c1[*ip++]++, c2[*ip++]++, c3[*ip++]++, c4[*ip++]++, c5[*ip++]++, c6[*ip++]++, c7[*ip++]++; 
  while(ip < in+inlen) 
    c0[*ip++]++; 
  int i;for(i = 0; i < 256; i++) count[i] = c0[i]+c1[i]+c2[i]+c3[i]+c4[i]+c5[i]+c6[i]+c7[i];

  RET;
}

  #ifdef __SSE4_1__
int count_sse4(unsigned char *in, unsigned inlen) { 
  int i;
  unsigned count[256]={0};
  unsigned c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0}; 
  unsigned char *ip;

  for(ip = in; ip != in+(inlen&~(16-1)); ip += 16) {
    __m128i vc = _mm_loadu_si128((__m128i*)ip);
    c0[_mm_extract_epi8(vc,  0)]++;
    c1[_mm_extract_epi8(vc,  1)]++;
    c2[_mm_extract_epi8(vc,  2)]++;
    c3[_mm_extract_epi8(vc,  3)]++;
    c0[_mm_extract_epi8(vc,  4)]++;
    c1[_mm_extract_epi8(vc,  5)]++;
    c2[_mm_extract_epi8(vc,  6)]++;
    c3[_mm_extract_epi8(vc,  7)]++;
    c0[_mm_extract_epi8(vc,  8)]++;
    c1[_mm_extract_epi8(vc,  9)]++;
    c2[_mm_extract_epi8(vc, 10)]++;
    c3[_mm_extract_epi8(vc, 11)]++;
    c0[_mm_extract_epi8(vc, 12)]++;
    c1[_mm_extract_epi8(vc, 13)]++;
    c2[_mm_extract_epi8(vc, 14)]++;
    c3[_mm_extract_epi8(vc, 15)]++;
  }
  while(ip < in+inlen) 
    c0[*ip++]++; 
  for(i = 0; i < 256; i++) 
    count[i] = c0[i]+c1[i]+c2[i]+c3[i];

  RET;
}

int count_sse8(unsigned char *in, unsigned inlen) { 
  int i;
  unsigned count[256]={0};
  unsigned c0[256]={0},c1[256]={0},c2[256]={0},c3[256]={0},c4[256]={0},c5[256]={0},c6[256]={0},c7[256]={0}; 
  unsigned char *ip;

  for(ip = in; ip != in+(inlen&~(16-1)); ip += 16) {
    __m128i vc = _mm_loadu_si128((__m128i*)ip);
    c0[_mm_extract_epi8(vc,  0)]++;
    c1[_mm_extract_epi8(vc,  1)]++;
    c2[_mm_extract_epi8(vc,  2)]++;
    c3[_mm_extract_epi8(vc,  3)]++;
    c4[_mm_extract_epi8(vc,  4)]++;
    c5[_mm_extract_epi8(vc,  5)]++;
    c6[_mm_extract_epi8(vc,  6)]++;
    c7[_mm_extract_epi8(vc,  7)]++;
    c0[_mm_extract_epi8(vc,  8)]++;
    c1[_mm_extract_epi8(vc,  9)]++;
    c2[_mm_extract_epi8(vc, 10)]++;
    c3[_mm_extract_epi8(vc, 11)]++;
    c4[_mm_extract_epi8(vc, 12)]++;
    c5[_mm_extract_epi8(vc, 13)]++;
    c6[_mm_extract_epi8(vc, 14)]++;
    c7[_mm_extract_epi8(vc, 15)]++;
  }
  while(ip < in+inlen) c0[*ip++]++; 
  for(i = 0; i < 256; i++) 
    count[i] = c0[i]+c1[i]+c2[i]+c3[i]+c4[i]+c5[i]+c6[i]+c7[i];

  RET;
}
  #endif

#define TMPRINT(__x) { tm_t tc = tmtime()-t0; printf("%s \t%7.2f %d\n", __x, (double)(tc>=0.000001?(((double)n/1048576.0)/(((double)tc/1)/TM_T)):0.0),r); t0=tminit(); }

int main(int argc, char *argv[]) {
  if(argc < 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    exit(-1);
  }
  FILE *fi = fopen(argv[1],  "rb"); 
  if(!fi ) perror(argv[1]), exit(1);  
  printf("'%s'\n", argv[1]);

  fseek(fi, 0, SEEK_END); 
  long long flen = ftell(fi); 
  fseek(fi, 0, SEEK_SET);

  if(flen > (1<<30)) flen = 1<<30;
  int n = flen; unsigned char *in = malloc(n); 
  if(!in) { fprintf(stderr, "malloc error\n"); exit(-1); }
  n = fread(in, 1, n, fi);
  fclose(fi);
  if(n <= 0) exit(0); 
  
  { 
    int r;
    tm_t t0 = tminit(); 
      #ifdef __SSE4_1__
    r = count_sse4(in,n); TMPRINT("sse4");
    r = count_sse8(in,n); TMPRINT("sse8");
      #endif
    r = count_8(   in,n); TMPRINT("count8");
    r = count_4(   in,n); TMPRINT("count4");
    r = count_1(   in,n); TMPRINT("count1");
  }
  free(in); 
}
