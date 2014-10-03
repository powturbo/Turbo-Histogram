TurboHist
=========

Fastest Histogram Construction
- ~1.3 clocks per byte (i7-2600k at 3.4GHz, data size = 64k)

# Benchmark:
i7-2600k at 4.5GHz, gcc 4.9, ubuntu 14.10.
- Practical benchmark with large files.

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 
<table>
<tr><th>Function</th><th>Uniform: Time MB/s</th><th>Skewed: Time MB/s</th></tr>
<tr><th>hist_4_128</th><th>2536.99</th><th>2513.40</th></tr>
<tr><th>hist_8_128</th><th>2528.18</th><th>2293.51</th></tr>
<tr><th>hist_4_32</th><th>2491.44</th><th>2429.87</th></tr>
<tr><th>hist_4_64</th><th>2234.26</th><th>2232.57</th></tr>
<tr><th>hist_8_64</th><th>2265.53</th><th>2018.01</th></tr>
<tr><th>hist_8_8</th><th>2102.43</th><th>1840.52</th></tr>
<tr><th>hist_4_8</th><th>2157.40</th><th>2030.37</th></tr>
<tr><th>hist_1_8</th><th>1793.55</th><th>882.48</th></tr>
</table>

## Compile:
  gcc -O2 -msse4.1 turbohist.c -o turbohist

## Usage:
  turbohist file
  
