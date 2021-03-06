TurboHist: Fastest Histogram Construction [![Build Status](https://travis-ci.org/powturbo/Turbo-Histogram.svg?branch=master)](https://travis-ci.org/powturbo/Turbo-Histogram)
=========================================

- ~1.2 clocks per byte
- 100% C (& C++ compatible) without inline assembly
- Both 32 and 64 bits supported
- Portable scalar functions faster than SIMD functions
- More than **3 times** faster than naive solution
- :new: more faster, beats even other fast assembler functions

# Benchmark:
- Single thread
- Realistic and practical benchmark with large files.
- No PURE cache benchmark

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 - Accurate benchmarking with command "turbohist file -I15"

###### Benchmark Intel CPU: Skylake i7-6700 3.4GHz gcc 9.2
|Uniform: Time MB/s|Skewed: Time MB/s|Function 2019.11|
|----------:|----------:|-------------|
|**2823**|**2823**|**hist_8_32**
|2817|2679|hist_4_32|
|2780|2778|hist_8_128|
|2781|2661|hist_4_128|
|2735|2734|hist_8_64|
|2731|2616|hist_4_64|
|2433|2432|count2x64c|
|2173|2050|hist_4_8|
|2166|1897|hist_8_8|
|1840|909|hist_1_8|

###### Benchmark ARM: ARMv8 A73-ODROID-N2 1.8GHz
|Uniform: Time MB/s|Skewed: Time MB/s|Function 2019.11|
|-----:|------:|----------|
|**719**|**717**|**hist_4_32**|
|  692| 700	|hist_8_32|
|  686| 689	|hist_8_64|
|  546|548 |	hist_8_128
|  572|573 |	hist_4_128
|  709| 711|	hist_4_64|
|  667| 671|	hist_8_8|
|  639| 641 |	hist_4_8|
|  597| 461|	hist_1_8|

(**bold** = pareto)  MB=1.000.000

## Compile:


        make

## Usage:


        turbohist file

### Environment:
###### OS/Compiler (32 + 64 bits):
- Windows: MinGW-w64 makefile
- Linux amd/intel: GNU GCC (>=4.6)
- Linux amd/intel: Clang (>=3.2) 
- Linux arm: aarch64 ARMv8:  gcc (>=6.3)
- MaxOS: XCode (>=9)
- PowerPC ppc64le: gcc (>=8.0)

Last update: 20 Aug 2020

