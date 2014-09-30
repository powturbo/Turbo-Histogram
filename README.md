turbohist
=========

Fast Histogram Construction

# Compile:
  gcc -w -O2 -msse4.1 histcalc.c -o histcalc

# Usage:
  histcalc <input file >
  
# Benchmark:

## Uniform distribution: enwik9 
(http://mattmahoney.net/dc/text.html)

sse4 	 429.566 ms
sse8 	 429.159 ms
count8 	 485.583 ms
count4 	 484.253 ms
count1 	 568.871 ms

## Skewed distribution: enwik9 bwt  generated w. libdivsufsort

sse4 	 432.523 ms
sse8 	 495.999 ms
count8 	 553.712 ms
count4 	 511.585 ms
count1 	1156.283 ms
