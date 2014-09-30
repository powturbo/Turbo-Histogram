turbohist
=========

Fast Histogram Construction

# Compile:
  gcc -w -O2 -msse4.1 histcalc.c -o histcalc

# Usage:
  histcalc <input file >
  
# Benchmark:
i7-2600k

## Uniform distribution: enwik9 (http://mattmahoney.net/dc/text.html)
<table>
  <tr>
<th>Function</th><th>Time in ms</th>
  </tr>
  <tr>
<th>sse4</th><th>429.566</th>
  </tr>
  <tr>
<th>sse8</th><th>429.159</th>
  </tr>
  <tr>
<th>count8</th><th>485.583</th>
  </tr>
  <tr>
<th>count4</th><th>484.253</th>
  </tr>
  <tr>
<th>count1</th><th>568.871</th>
  </tr>
</table>

## Skewed distribution: enwik9 bwt  generated w. libdivsufsort
<table>
  <tr>
<th>Function</th><th>Time in ms</th>
  </tr>
  <tr>
<th>sse4 	 <th>432.523
  </tr>
  <tr>
<th>sse8 	 <th>495.999
  </tr>

  <tr>
<th>count8 	 <th>553.712
  </tr>
  <tr>
<th>count4 	 <th>511.585
  </tr>
  <tr>
<th>count1 	<th>1156.283
  </tr>
</table>
