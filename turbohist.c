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
// Turbo histogram benchmark 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
  #ifdef __APPLE__
#include <sys/malloc.h>
  #else
#include <malloc.h>
  #endif
  #ifdef _MSC_VER
#include "vs/getopt.h"
  #else 
#include <getopt.h>
#include <unistd.h>   
  #endif
#include "conf.h"
#include "time_.h"

#include "turbohist_.c"
  #ifdef _RYG
#include "histotest.cpp"
  #endif 

NOINLINE void libmemcpy(unsigned char *dst, unsigned char *src, int len) {
  void *(*memcpy_ptr)(void *, const void *, size_t) = memcpy;
  if (time(NULL) == 1)
    memcpy_ptr = NULL;
  memcpy_ptr(dst, src, len);
}
    
void usage(char *pgm) {
  fprintf(stderr, "\nTurboHist Copyright (c) 2013-2022 Powturbo %s\n", __DATE__);
  fprintf(stderr, "Usage: %s [options] [file]\n", pgm);
  fprintf(stderr, "Benchmark:\n");
  fprintf(stderr, " -I#      # = Number of runs (default=3)\n");
  fprintf(stderr, " -z       set the read buffer to zeros\n");
  fprintf(stderr, "Ex. ./turbohist file -I15\n");
  fprintf(stderr, "    ./turbohist file -I15 -z\n");
  fprintf(stderr, "    ./turbohist file -e1,4,8,15 -I15\n");
  exit(0);
}      
          
int check(unsigned *cnt, unsigned n, unsigned *scnt) { unsigned i; for(i=0;i<256;i++) if(cnt[i]!=scnt[i]) { printf("Error sum at %d ", i); return 0; } printf(" %s", TM_MBS); }

int bench(unsigned char *in, unsigned n, unsigned *cnt, unsigned id, unsigned *scnt) {
  switch(id) {
	case  1: TMBENCH(" 1:hist_1_8   naiv    8 bits", hist_1_8(   in, n, cnt),n);			break; 
	case  2: TMBENCH(" 2:hist_4_8   4 bins/ 8 bits", hist_4_8(   in, n, cnt),n);			break;
	case  3: TMBENCH(" 3:hist_8_8   8 bins/ 8 bits", hist_8_8(   in, n, cnt),n);			break;
	case  4: TMBENCH(" 4:hist_4_32  4 bins/32 bits", hist_4_32(  in, n, cnt),n);			break;
	case  5: TMBENCH(" 5:hist_8_32  8 bins/32 bits", hist_8_32(  in, n, cnt),n);			break;
 	case  6: TMBENCH(" 6:hist_4_64  4 bins/64 bits", hist_4_64(  in, n, cnt),n);			break;
	case  7: TMBENCH(" 7:hist_8_64  8 bins/64 bits", hist_8_64(  in, n, cnt),n);			break;
 	case  8: TMBENCH(" 8:histr_4_64 4/64+run      ", histr_4_64( in, n, cnt),n);			break;
	case  9: TMBENCH(" 9:histr_8_64 8/64+run      ", histr_8_64( in, n, cnt),n);			break;
      #ifdef __ARM_NEON
	case 10: TMBENCH("10:hist_4_128 4 bins/neon   ", hist_4_128( in, n, cnt),n);			break;
	case 11: TMBENCH("11:hist_8_128 8 bins/neon   ", hist_8_128( in, n, cnt),n);			break;
      #else
	case 10: TMBENCH("10:hist_4_128 4 bins/sse4.1 ", hist_4_128( in, n, cnt),n);			break;
	case 11: TMBENCH("11:hist_8_128 8 bins/sse4.1 ", hist_8_128( in, n, cnt),n);			break;
      #endif
	  #ifdef __AVX2__
	case 12: TMBENCH("12:hist_4_256 4 bins/avx2   ", hist_4_256( in, n, cnt),n);			break;
	case 13: TMBENCH("13:hist_8_256 8 bins/avx2   ", hist_8_256( in, n, cnt),n);			break;
	  #endif
      #ifdef __x86_64
	case 15: TMBENCH("15:hist_8_64asm inline asm  ", hist_8_64a( in, n, cnt),n);		    break;
	  #endif
	  #ifdef _COUNTBENCH
	case 18: TMBENCH("18:count2x64    inline asm  ", count2x64(  in, n, cnt),n);            break;  
//	case 19: TMBENCH("19:count2x64c               ", count2x64c( in, n, cnt),n);            break;
 	  #endif 
      #ifdef _RYG	  
	case 20: TMBENCH("20:histo_ref                ", histo_ref(             cnt, in, n),n); break; 
	case 21: TMBENCH("21:histo_cpp_1x             ", histo_cpp_1x(          cnt, in, n),n); break; 
	case 22: TMBENCH("22:histo_cpp_2x             ", histo_cpp_2x(          cnt, in, n),n); break; 
	case 23: TMBENCH("23:histo_cpp_4x             ", histo_cpp_4x(          cnt, in, n),n); break; 
	case 24: TMBENCH("24:histo_asm_scalar4        ", histo_asm_scalar4(     cnt, in, n),n); break; 
	case 25: TMBENCH("25:histo_asm_scalar8        ", histo_asm_scalar8(     cnt, in, n),n); break; 
	case 26: TMBENCH("26:histo_asm_scalar8_var    ", histo_asm_scalar8_var( cnt, in, n),n); break; 
	case 27: TMBENCH("27:histo_asm_scalar8_var2   ", histo_asm_scalar8_var2(cnt, in, n),n); break;
	case 28: TMBENCH("28:histo_asm_scalar8_var3   ", histo_asm_scalar8_var3(cnt, in, n),n); break;
	case 29: TMBENCH("29:histo_asm_scalar8_var4   ", histo_asm_scalar8_var4(cnt, in, n),n); break;
	case 30: TMBENCH("30:histo_asm_scalar8_var5   ", histo_asm_scalar8_var5(cnt, in, n),n); break;
	case 31: TMBENCH("31:histo_asm_sse4           ", histo_asm_sse4(        cnt, in, n),n); break;
	    #ifdef __AVX2__
    case 37: TMBENCH("37:histo_asm_avx256_8x_1    ", histo_asm_avx256_8x_1( cnt, in, n),n); break;
	case 38: TMBENCH("38:histo_asm_avx256_8x_2    ", histo_asm_avx256_8x_2( cnt, in, n),n); break;
	case 39: TMBENCH("39:histo_asm_avx256_8x_3    ", histo_asm_avx256_8x_3( cnt, in, n),n); break;
	    #endif	 
	  #endif
	case 32: { unsigned char *cpy = malloc(n); if(cpy) { TMBENCH("32:memcpy                   ", libmemcpy(cpy, in, n),n); free(cpy); printf(" %s", TM_MBS); } } return 0; break;
    #define ID_LAST 32	
    default: return 0;
  }        
  check(cnt,n,scnt);
  return 1; 
} 

int main(int argc, char *argv[]) { 
  unsigned char *finame = argv[1], *scmd = NULL, *in; 
  unsigned n, fno, zero=0, scnt[256], cnt[256]; 
        
  int c, digit_optind = 0;
  for(;;) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      { "help", 	0, 0, 'h'},
      { 0, 		    0, 0, 0}
    };
    if((c = getopt_long(argc, argv, "e:hI:z", long_options, &option_index)) == -1) break;
    switch(c) { 
      case 0:
        printf("Option %s", long_options[option_index].name);
        if(optarg) printf (" with arg %s", optarg);  printf ("\n");
        break;
      case 'I': if((tm_Rep  = atoi(optarg))<=0) tm_rep =tm_Rep =1; break;
	  case 'z': zero++; break;
      case 'e': scmd = optarg; break;
	  case 'h':
      default: 
        usage(argv[0]);
        exit(0); 
    }  
  }   
           
  printf("\nTurboHist Copyright (c) 2013-2022 Powturbo %s\n", __DATE__);
  char _scmd[33];
  sprintf(_scmd, "1-%d", ID_LAST);                         

  for(fno = optind; fno < argc; fno++) {
    finame = argv[fno];
  
    FILE *fi = fopen(finame,  "rb"); 
    if(!fi) perror(finame), exit(1); 							// printf("'%s'\n", finame);
   
    fseek(fi, 0, SEEK_END); 
    long long flen = ftell(fi); 
    fseek(fi, 0, SEEK_SET);  
  
    if(flen > GB) flen = GB;
    n = flen; 
    if(!(in = (unsigned char*)malloc(n))) 
      printf("malloc error\n"), exit(-1);
    n = fread(in, 1, n, fi);
    fclose(fi);
    if(n <= 0)   
      exit(0);     
  
    if(zero) memset(in, 0, n);                                                                          
    int i; hist_1_8(in, n, scnt);  // first run
	unsigned char *p = (scmd && (scmd[0] != '0' || scmd[1]))?scmd:_scmd;
    do { 
      int id = strtoul(p, &p, 10),idx = id, i;
      if(id >= 0) {    
        while(isspace(*p)) p++; if(*p == '-') { if((idx = strtoul(p+1, &p, 10)) < id) idx = id; if(idx > ID_LAST) idx = ID_LAST; } //printf("ID=%d,%d ", id, idx);
        for(i = id; i <= idx; i++) {
          if(bench(in, n, cnt, i, scnt)) printf("\t%s\n", finame);  
        }			  
      }        
    } while(*p++);
    printf("\n");
    free(in);
  }
}
