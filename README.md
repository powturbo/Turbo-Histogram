TurboHist
=========

Fastest Histogram Construction
- ~1.6 clocks per byte 
- (Tested on i7-2600k at 3.4GHz, data size = 64k)

# Compile:
  gcc -w -O2 -msse4.1 turbohist.c -o turbohist

# Usage:
  turbohist file
  
# Benchmark:
i7-2600k at 4.5GHz, gcc 4.9, ubuntu 14.10.
- Practical benchmark with large files.

### - Uniform distribution: 
[enwik9](http://mattmahoney.net/dc/text.html)
<table>
  <tr><th>Function</th><th>Time MB/s</th></tr>
  <tr><th>sse4</th><th>2370.37</th></tr>
  <tr><th>sse8</th><th>2367.82</th></tr>
  <tr><th>count8</th><th>2102.41</th></tr>
  <tr><th>count4</th><th>2107.50</th></tr>
  <tr><th>count1</th><th>1783.99</th></tr>
</table>

### - Skewed distribution: 
enwik9 bwt  generated w. libdivsufsort
<table>
  <tr><th>Function</th><th>Time MB/s</th></tr>
  <tr><th>sse4<th>2324.62</th></tr>
  <tr><th>sse8<th>2056.03</th></tr>
  <tr><th>count8<th>1839.18</th></tr>
  <tr><th>count4<th>1991.58</th></tr>
  <tr><th>count1<th>882.99</th></tr>
</table>
