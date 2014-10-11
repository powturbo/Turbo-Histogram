TurboHist
=========

Fastest Histogram Construction
- ~1.3 clocks per byte
- 100% C, without inline assembly
- Both 32 and 64 bits supported

# Benchmark:
i7-2600k at 4.5GHz, gcc 4.9, ubuntu 14.10.
- Single thread
- Realistic and practical benchmark with large files. 

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 
<table>
<tr><th>Function</th><th>Uniform: Time MB/s</th><th>Skewed: Time MB/s</th></tr>
<tr><th>hist_8_32</th><th>2758.61</th><th>2741.35</th></tr>
<tr><th>hist_4_32</th><th>2745.88</th><th>2594.32</th></tr>
<tr><th>hist_8_128</th><th>2714.06</th><th>2709.78</th></tr>
<tr><th>hist_4_128</th><th>2710.62</th><th>2650.48</th></tr>
<tr><th>hist_8_64</th><th>2686.89</th><th>2657.72</th></tr>
<tr><th>hist_4_64</th><th>2623.87</th><th>2523.67</th></tr>
<tr><th>hist_8_8</th><th>2349.73</th><th>2333.46</th></tr>
<tr><th>hist_4_8</th><th>2211.00</th><th>2082.84</th></tr>
<tr><th>hist_1_8</th><th>1870.33</th><th>926.88</th></tr>
<tr><th>count2x64</th><th>2838.60</th><th>2837.06</th></tr>
</table>

## Compile:
  gcc -O3 -march=native turbohist.c -o turbohist

## Usage:
  turbohist file

## Reference:
Countbench: https://github.com/nkurz/countbench (including "count2x64" with inline assembly)

