# EXPINT

## Background
The series function of expint is to calculate Exponential Integral. It is in 
the std library of GNU but not introduced into LLVM's std library.

To support project building for LLVM, expint is added in the 
third_party library with minor modification. std::expint from GNU in not used 
anymore.

## Source
This implementation is from a PR of LLVM.
 https://reviews.llvm.org/D59937

## Modification

- Add LLVM's license and WFA's license in header file
- Remove compiler specified flags
- Rename variables without prefix underscores
- Reformat comments
- Change namespace from `std` into `llvm`

## Test

Values in test cases are from GNU's std::expint


<img src="https://latex.codecogs.com/svg.latex?\Large&space;\mathrm{Ei} (x) " title="\Large x=\frac{-b\pm\sqrt{b^2-4ac}}{2a}" />