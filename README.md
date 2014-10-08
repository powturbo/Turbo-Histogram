TurboHist
=========

Fastest Histogram Construction
- ~1.3 clocks per byte
- 100% C without inline assembly

# Benchmark:
i7-2600k at 4.5GHz, gcc 4.9, ubuntu 14.10.
- Single thread
- Realistic and practical benchmark with large files. 

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 
<table>
<tr><th>Function</th><th>Uniform: Time MB/s</th><th>Skewed: Time MB/s</th></tr>
<tr><th>hist_8_32</th><th>2702.92</th><th>2692.51</th></tr>
<tr><th>hist_4_128</th><th>2568.36</th><th>2513.40</th></tr>
<tr><th>hist_8_128</th><th>2572.15</th><th>2564.42</th></tr>
<tr><th>hist_4_32</th><th>2604.89</th><th>2505.70</th></tr>
<tr><th>hist_4_64</th><th>2338.06</th><th>2344.03</th></tr>
<tr><th>hist_8_64</th><th>2345.17</th><th>2395.45</th></tr>
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

Blog: http://fastcompression.blogspot.com/2014/09/counting-bytes-fast-little-trick-from.html
