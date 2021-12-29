TurboHist: Fastest Histogram Construction
=========================================

- **~0.18 - 0.90 cycles per byte**
- 100% C (C++ compatible header) without inline assembly
- Both 32 and 64 bits supported
- Portable scalar functions faster than SIMD functions
- **Up to 22 times** faster than naive solution
- :new: (2022.01) more faster, beats even other very fast assembler functions

# Benchmark:
- Single thread
- Realistic and practical benchmark with large files.
- No PURE cache benchmark

#### - Uniform/Skewed distribution: 
 - Uniform: [enwik9](http://mattmahoney.net/dc/text.html)
 - Skewed: enwik9 bwt generated w. libdivsufsort
 - 1GB zeros 
 - Accurate benchmarking with command "turbohist file -I15"

###### Benchmark Intel CPU: i7-9700K 3.6GHz gcc 11.2
Uniform distribution - enwik9 Text file, size=1.000.0000.000
| Function                   | MB/s   |Cycle/Byte|Language  |Package  |
|----------------------------|-------:|---------:|----------|---------|
| 1:hist_1_8   naiv    8 bits| 2761.01|1.3423    |C         |TurboHist|
| 2:hist_4_8   4 bins/ 8 bits| 2725.92|1.3249|C|TurboHist|
| 3:hist_8_8   8 bins/ 8 bits| 2850.05|1.2627|C|TurboHist|
| 4:hist_4_32  4 bins/32 bits| 3691.02|0.9660|C|TurboHist|
| 5:hist_8_32  8 bins/32 bits| 3867.26|0.9561|C|TurboHist|
| 6:hist_4_64  4 bins/64 bits|4040.55|0.9103|C|TurboHist|
| 7:hist_8_64  8 bins/64 bits|**4053.37**|**0.9035**|C|TurboHist|
| 8:histr_4_64 4/64+run      | 3915.85|0.9668|C|TurboHist|
| 9:histr_8_64 8/64+run      | 3916.51|0.9286|C|TurboHist|
|10:hist_4_128 4 bins/sse4.1 | 3643.20|1.0081|C|TurboHist|
|11:hist_8_128 8 bins/sse4.1 | 3607.06|0.9845|C|TurboHist|
|12:hist_4_256 4 bins/avx2   | 3522.27|1.0195|C|TurboHist|
|13:hist_8_256 8 bins/avx2   | 3542.25|1.0366|C|TurboHist|
|15:hist_8_64asm inline asm  |**4161.87**|**0.8787**|inline asm|TurboHist|
|18:count2x64    inline asm  | 3963.91|0.9172|inline asm|Countbench|
|20:histo_ref                | 2702.57|1.3567|C|[Ryg](https://gist.github.com/rygorous/a86a5cf348922cdea357c928e32fc7e0)|
|21:histo_cpp_1x             | 1876.13|1.8236|C|Ryg|
|22:histo_cpp_2x             | 2664.78|1.5935|C|Ryg|
|23:histo_cpp_4x             | 2817.77|1.2944|C|Ryg|
|24:histo_asm_scalar4        | 3130.08|1.1609|asm|Ryg|
|25:histo_asm_scalar8        | 3353.08|1.0636|asm|Ryg|
|26:histo_asm_scalar8_var    | 3704.88|0.9856|asm|Ryg|
|27:histo_asm_scalar8_var2   | 4085.48|0.8913|asm|Ryg|
|28:histo_asm_scalar8_var3   | 4132.54|0.8870|asm|Ryg|
|29:histo_asm_scalar8_var4   | 4083.92|0.8970|asm|Ryg|
|30:histo_asm_scalar8_var5   | 4002.21|0.9025|asm|Ryg|
|31:histo_asm_sse4           | 3153.01|1.1445|asm|Ryg|
|32:memcpy                   |13724.29|0.2698|C|

Skewed distribution - enwik9.bwt Text file, size=1.000.0000.000
| Function                   | MB/s   |Cycle/Byte|Language  |
|----------------------------|-------:|---------:|----------|
| 1:hist_1_8   naiv    8 bits| 1170.89|3.0642|C|TurboHist|
| 2:hist_4_8   4 bins/ 8 bits| 2707.74|1.3321|C|TurboHist|
| 3:hist_8_8   8 bins/ 8 bits| 2804.08|1.3208|C|TurboHist|
| 4:hist_4_32  4 bins/32 bits| 3118.54|1.1402|C|TurboHist|
| 5:hist_8_32  8 bins/32 bits| 3780.16|0.9714|C|TurboHist|
| 6:hist_4_64  4 bins/64 bits| 3646.25|0.9980|C|TurboHist|
| 7:hist_8_64  8 bins/64 bits| 3941.96|0.9282|C|TurboHist|
| 8:histr_4_64 4/64+run      | 5061.62|0.7270|C|TurboHist|
| 9:histr_8_64 8/64+run      |**5135.29**|**0.7229**|C|TurboHist|
|10:hist_4_128 4 bins/sse4.1 | 3535.36|1.0365|C|TurboHist|
|11:hist_8_128 8 bins/sse4.1 | 3654.41|0.9791|C|TurboHist|
|12:hist_4_256 4 bins/avx2   | 3329.87|1.1022|C|TurboHist|
|13:hist_8_256 8 bins/avx2   | 3540.36|1.0343|C|TurboHist|
|15:hist_8_64asm inline asm  | 4047.74|0.9013|inline asm|TurboHist|
|18:count2x64    inline asm  | 3969.92|0.9262|inline asm|[Countbench](https://github.com/nkurz/countbench)|
|20:histo_ref                | 1182.61|3.0718|C|Ryg|
|21:histo_cpp_1x             | 1213.42|2.9748|C|Ryg|
|22:histo_cpp_2x             | 2115.60|1.7373|C|Ryg|
|23:histo_cpp_4x             | 1801.97|2.0024|C|Ryg|
|24:histo_asm_scalar4        | 3092.87|1.1561|asm|Ryg|
|25:histo_asm_scalar8        | 3203.95|1.1139|asm|Ryg|
|26:histo_asm_scalar8_var    | 3460.45|1.0422|asm|Ryg|
|27:histo_asm_scalar8_var2   | 3659.61|0.9878|asm|Ryg|
|28:histo_asm_scalar8_var3   | 3769.96|0.9569|asm|Ryg|
|29:histo_asm_scalar8_var4   | 3996.75|0.8905|asm|Ryg|
|30:histo_asm_scalar8_var5   | 4642.10|0.7719|asm|Ryg|
|31:histo_asm_sse4           | 3091.36|1.1670|asm|Ryg|
|32:memcpy                   |15594.28|0.2412|C|
 
All zeros: size=1.000.0000.000
| Function                   | MB/s   |Cycle/Byte|Language  |
|----------------------------|-------:|---------:|----------|
| 1:hist_1_8   naiv    8 bits|  877.27|4.0805|C|TurboHist| 
| 2:hist_4_8   4 bins/ 8 bits| 2650.84|1.3485|C|TurboHist| 
| 3:hist_8_8   8 bins/ 8 bits| 2743.40|1.2994|C|TurboHist| 
| 4:hist_4_32  4 bins/32 bits| 2978.83|1.2006|C|TurboHist| 
| 5:hist_8_32  8 bins/32 bits| 3775.45|0.9555|C|TurboHist| 
| 6:hist_4_64  4 bins/64 bits| 3411.11|1.0530|C|TurboHist| 
| 7:hist_8_64  8 bins/64 bits| 3928.09|0.9342|C|TurboHist| 
| 8:histr_4_64 4/64+run      |18998.87|0.1868|C|TurboHist| 
| 9:histr_8_64 8/64+run      |**19629.28**|**0.1869**|C|TurboHist| 
|10:hist_4_128 4 bins/sse4.1 | 3365.40|1.0717|C|TurboHist| 
|11:hist_8_128 8 bins/sse4.1 | 3632.61|0.9950|C|TurboHist| 
|12:hist_4_256 4 bins/avx2   | 3112.15|1.1576|C|TurboHist| 
|13:hist_8_256 8 bins/avx2   | 3497.08|1.0205|C|TurboHist| 
|15:hist_8_64asm inline asm  |4089.97|0.8817|inline asm|TurboHist| 
|18:count2x64    inline asm  | 3881.98|0.9158|inline asm|Countbench| 
|20:histo_ref                |  882.93|4.1072|C|Ryg| 
|21:histo_cpp_1x             |  873.20|4.1069|C|Ryg| 
|22:histo_cpp_2x             | 1720.19|2.0961|C|Ryg| 
|23:histo_cpp_4x             | 1866.99|2.0817|C|Ryg| 
|24:histo_asm_scalar4        | 2995.84|1.1942|asm|Ryg| 
|25:histo_asm_scalar8        | 3107.30|1.1618|asm|Ryg| 
|26:histo_asm_scalar8_var    | 3288.67|1.1143|asm|Ryg| 
|27:histo_asm_scalar8_var2   | 3290.92|1.0957|asm|Ryg| 
|28:histo_asm_scalar8_var3   | 3707.41|0.9763|asm|Ryg| 
|29:histo_asm_scalar8_var4   | 3988.01|0.9019|asm|Ryg| 
|30:histo_asm_scalar8_var5   |14076.09|0.2564|asm|Ryg| 
|31:histo_asm_sse4           | 3020.32|1.1975|asm|Ryg| 
|32:memcpy                   |14057.53|0.2636|C|

(**bold** = pareto)  MB=1.000.000
- [Ryg](https://gist.github.com/rygorous/a86a5cf348922cdea357c928e32fc7e0) <br/>
- [Countbench](https://github.com/nkurz/countbench)

## Compile:


        make
     or
        make AVX2=1

## Usage:


        turbohist [-e#] file [-I#] [-z]
        options:
        -e#     # = function numbers separated by ,
        -I#     # = number of iteration
                set to -I15 for accurate timings  
        -z      set read buffer to zeros
                
Examples:
        ./turbohist file
        ./turbohist -e1,7,9

### Environment:
###### OS/Compiler (32 + 64 bits):
- Windows: MinGW-w64 makefile
- Linux amd/intel: GNU GCC (>=4.6)
- Linux amd/intel: Clang (>=3.2) 
- Linux arm: aarch64 ARMv8:  gcc (>=6.3)
- MaxOS: XCode (>=9) + Apple M1
- PowerPC ppc64le: gcc (>=8.0)

Last update: 01 JAN 2022
