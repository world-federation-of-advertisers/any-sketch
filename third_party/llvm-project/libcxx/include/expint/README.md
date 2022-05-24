# EXPINT

## Background
The series function of expint is to calculate Exponential Integral. It is in 
GNU stdlibc++, but not introduced into LLVM libc++ 13 or older version.

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
