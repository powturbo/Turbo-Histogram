TurboHist: Fastest Histogram Construction [![Build Status](https://travis-ci.org/powturbo/TurboHist.svg?branch=master)](https://travis-ci.org/powturbo/TurboHist)
=========================================

- ~1.2 clocks per byte
- 100% C (& C++ compatible) without inline assembly
- Both 32 and 64 bits supported
- Portable scalar functions faster than SIMD functions
- Up to **3 times** faster than naive solution
- :new: more faster, beats even other fast assembler functions

# Benchmark:
- CPU Sandy Bridge i7-2600k at 4.4GHz, gcc 6.2
- Single thread
- Realistic and practical benchmark with large files.
- No PURE cache benchmark

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 - Accurate benchmarking with command "turbohist file -I15"


|Function|Uniform: Time MB/s|Skewed: Time MB/s|
|**hist_8_32**|**2823**|**2823**|
|hist_4_32|2817|2679|
|count2x64|2788|2788|
|hist_8_128|2780|2778|
|hist_4_128|2781|2661|
|hist_8_64|2735|2734|
|hist_4_64|2730.79|2616|
|count2x64c|2433|2432|
|hist_4_8|2173.40|2050|
|hist_8_8|2166|1897|
|hist_1_8|1840.12|909|

 (bold = pareto)  MB=1.000.000

## Compile:


        make

## Usage:


        turbohist file

### Environment:
###### OS/Compiler (32 + 64 bits):
- Linux: GNU GCC (>=4.6) 
- clang (>=3.2) 
- Windows: MinGW
- Windows: Visual Studio 2015
- ARM 64 bits/ gcc 

### References:
Countbench: https://github.com/nkurz/countbench (including "count2x64" with inline assembly)

Last update: 24 DEC 2016

