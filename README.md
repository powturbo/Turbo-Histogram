TurboHist
=========

Fastest Histogram Construction
- ~1.6 clocks per byte (i7-2600k at 3.4GHz, data size = 64k)

# Benchmark:
i7-2600k at 4.5GHz, gcc 4.9, ubuntu 14.10.
- Practical benchmark with large files.

#### - Uniform distribution: 
[enwik9](http://mattmahoney.net/dc/text.html)
<table>
  <tr><th>Function</th><th>Time MB/s</th></tr>
  <tr><th>sse4</th><th>2370.37</th></tr>
  <tr><th>sse8</th><th>2367.82</th></tr>
  <tr><th>count4u</th><th>2317.39</th></tr>
  <tr><th>count8l</th><th>2274.70</th></tr>
  <tr><th>count8</th><th>2102.41</th></tr>
  <tr><th>count4</th><th>2107.75</th></tr>
  <tr><th>count1</th><th>1795.67</th></tr>
</table>

#### - Skewed distribution: 
enwik9 bwt  generated w. libdivsufsort
<table>
  <tr><th>Function</th><th>Time MB/s</th></tr>
  <tr><th>sse4<th>2354.77</th></tr>
  <tr><th>sse8<th>2056.03</th></tr>
  <tr><th>count4u</th><th>2254.86</th></tr>
  <tr><th>count8l</th><th>2042.50</th></tr>
  <tr><th>count8<th>1841.82</th></tr>
  <tr><th>count4<th>1992.11</th></tr>
  <tr><th>count1<th>882.99</th></tr>
</table>

## Compile:
  gcc -w -O2 -msse4.1 turbohist.c -o turbohist

## Usage:
  turbohist file
  
