TurboHist: Fastest Histogram Construction
=========================================

- ~1.2 clocks per byte
- 100% C (& C++ compatible) without inline assembly
- Both 32 and 64 bits supported
- Portable scalar functions faster thand SIMD functions
- Up to **3 times** faster than naive solution

# Benchmark:
i7-2600k at 4.5GHz, gcc 5.1, ubuntu 15.04.
- Single thread
- Realistic and practical benchmark with large files.
- No PURE cache benchmark

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 
<table>
<tr><th>Function</th><th>Uniform: Time MB/s</th><th>Skewed: Time MB/s</th></tr>
<tr><th>hist_8_32</th><th>2758.61</th><th>2746.19</th></tr>
<tr><th>hist_4_32</th><th>2745.88</th><th>2594.32</th></tr>
<tr><th>hist_8_128</th><th>2714.06</th><th>2709.78</th></tr>
<tr><th>hist_4_128</th><th>2715.28</th><th>2650.48</th></tr>
<tr><th>hist_8_64</th><th>2697.89</th><th>2670.17</th></tr>
<tr><th>hist_4_64</th><th>2639.83</th><th>2553.63</th></tr>
<tr><th>hist_8_8</th><th>2349.73</th><th>2333.46</th></tr>
<tr><th>hist_4_8</th><th>2213.05</th><th>2082.84</th></tr>
<tr><th>hist_1_8</th><th>1882.86</th><th>926.88</th></tr>
<tr><th>count2x64</th><th>2838.60</th><th>2836.31</th></tr>
</table>

## Compile:
  cc -O3 -march=native turbohist.c -o turbohist

## OS/Compiler
  - Linux: gcc (>=4.6)
  - clang (>=3.2)
  - Windows: mingw-w64 (>=4.6)

## Usage:
  turbohist file

## Reference:
Countbench: https://github.com/nkurz/countbench (including "count2x64" with inline assembly)

