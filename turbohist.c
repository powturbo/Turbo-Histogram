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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

//--------------------------------------- Time ------------------------------------------------------------------------
#include <time.h>
typedef unsigned long long tm_t;
#define TM_T 1000000.0
  #ifdef _WIN32
#include <windows.h>
static LARGE_INTEGER tps;
static tm_t tmtime(void) { LARGE_INTEGER tm; QueryPerformanceCounter(&tm); return (tm_t)((double)tm.QuadPart*1000000.0/tps.QuadPart); }
static tm_t tminit() { QueryPerformanceFrequency(&tps); tm_t t0=tmtime(),ts; while((ts = tmtime())==t0); return ts; } 
  #else
static   tm_t tmtime(void)    { struct timespec tm; clock_gettime(CLOCK_MONOTONIC, &tm); return (tm_t)tm.tv_sec*1000000ull + tm.tv_nsec/1000; }
static   tm_t tminit()        { tm_t t0=tmtime(),ts; while((ts = tmtime())==t0); return ts; }
  #endif 
//---------------------------------------- bench --------------------------------------------------------------
#define MBS 		  1000000.0
#define TMBS(__l,__t) ((__t)>=0.000001?((double)(__l)/MBS)/((__t)/TM_T):0.0)
#define TM_TX         TM_T 
 
#define TMDEF unsigned tm_r,tm_R,tm_c; tm_t _t0,_tc,_ts; double _tmbs=0.0;
#define TMSLEEP do { tm_T = tmtime(); if(!tm_0) tm_0 = tm_T; else if(tm_T - tm_0 > tm_TX) { printf("S \b\b\b");fflush(stdout); sleep(tm_slp); tm_0=tmtime();} } while(0)
#define TMBEG(_c_, _tm_reps_, _tm_Reps_) \
  for(tm_c=_c_,tm_tm = (1ull<<63),tm_rm=1,tm_R=0,_ts=tmtime(); tm_R < _tm_Reps_; tm_R++) { printf("%8.2f %.2d_%d\b\b\b\b\b\b\b\b\b\b\b\b\b",_tmbs,tm_R+1,tm_c);fflush(stdout);\
    for(_t0 = tminit(), tm_r=0; tm_r < _tm_reps_;) {
 
#define TMEND(_len_) tm_T = tmtime(); tm_r++; if((_tc = (tm_T - _t0)) > tm_tx) break; }\
  if(_tc/(double)tm_r < (double)tm_tm/(double)tm_rm) { tm_tm = _tc; tm_rm=tm_r; tm_c++; double _d = (double)tm_tm/(double)tm_rm; _tmbs=TMBS(_len_, _d); } else if(_tc/tm_tm>1.2) TMSLEEP; if(tm_T-_ts > tm_TX) break;\
  if((tm_R & 7)==7) { sleep(tm_slp); _ts=tmtime(); } }

#define TMVARS static unsigned tm_repc = 1<<30, tm_Repc = 4, tm_repd = 1<<30, tm_Repd = 4, tm_rm, tm_slp = 25;\
               static tm_t     tm_tm, tm_tx = TM_TX, tm_TX = 30*TM_TX, tm_0, tm_T, tm_RepkT=24*3600*TM_TX;
//--------------------------------------------------------------------------------------------------------------
TMVARS; 

//#define COUNTBENCH  // https://github.com/nkurz/countbench 

#include "turbohist_.c"

#define GB (1<<30)

typedef unsigned (*FUNC)(unsigned char *in, unsigned n);
struct cod { FUNC func; char *s; };

struct cod cods[] = { 
  { hist_8_32,  "hist_8_32"  }, 
  { hist_4_32,  "hist_4_32"  }, 
    #if defined(__SSE4_1__) || defined(__ARM_NEON)
  { hist_8_128, "hist_8_128" }, 
  { hist_4_128, "hist_4_128" }, 
    #endif
    #ifdef __AVX2__
  { hist_8_256, "hist_8_256" }, 
  { hist_4_256, "hist_4_256" }, 
    #endif
  { hist_8_64,  "hist_8_64"  }, 
  { hist_4_64,  "hist_4_64"  }, 
  { hist_8_8,   "hist_8_8"   }, 
  { hist_4_8,   "hist_4_8"   }, 
  { hist_1_8,   "hist_1_8"   }, 
    #ifdef COUNTBENCH
  { count2x64,  "count2x64"  }, 
  { count2x64c, "count2x64c" },
    #endif
};

void usage(char *pgm) {
  fprintf(stderr, "\nTurboHist Copyright (c) 2013-2019 Powturbo %s\n", __DATE__);
  fprintf(stderr, "Usage: %s [options] [file]\n", pgm);
  fprintf(stderr, "Benchmark:\n");
  fprintf(stderr, " -i#      # = Minimum  iterations per run (default=auto)\n");
  fprintf(stderr, " -I#      # = Number of runs (default=4)\n");
  fprintf(stderr, "Check:\n");
  fprintf(stderr, " -s       print checksum\n");
  fprintf(stderr, "Ex. ./turbohist file -I15\n");
  exit(0);
} 

int main(int argc, char *argv[]) { unsigned char *in;int n,fno;
  char *finame = argv[1];  

  int c, digit_optind = 0;
  for(;;) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      { "help", 	0, 0, 'h'},
      { 0, 		    0, 0, 0}
    };
    if((c = getopt_long(argc, argv, "hsi:I:", long_options, &option_index)) == -1) break;
    switch(c) { 
      case 0:
        printf("Option %s", long_options[option_index].name);
        if(optarg) printf (" with arg %s", optarg);  printf ("\n");
        break;
      case 'i': if((tm_repc  = atoi(optarg))<=0) 
		          tm_repc=tm_Repc=1;         		 break;
      case 'I': tm_Repc  = atoi(optarg);       		 break;
      case 's': csum++;       		                 break;
	  case 'h':
      default: 
        usage(argv[0]);
        exit(0); 
    }
  }
  printf("\nTurboHist Copyright (c) 2013-2019 Powturbo %s\n", __DATE__);

  for(fno = optind; fno < argc; fno++) {
    finame = argv[fno];

    FILE *fi = fopen(finame,  "rb"); 
    if(!fi ) perror(finame), exit(1);  
    printf("'%s'\n", finame);

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
  
    int r,i, cr = hist_1_8(in, n); 
    TMDEF;
    for(i = 0; i < sizeof(cods)/sizeof(cods[0]); i++) {
      TMBEG(0, tm_repc, tm_Repc);
      r = cods[i].func(in, n); 
      TMEND(n);

      double tc = (double)tm_tm/(double)tm_rm;
      printf("%8.2f MB/s\t%s", TMBS(n,tc), cods[i].s);
      if(csum) {
        printf(" checksum=%d ", r);
        if(r != cr) { printf("Fatal error in '%s'\n", cods[i].s); exit(-1); }
      } 
      printf("\n");
      fflush(stdout);
    }  
    free(in);
  }
}

