# GOST-34112018-STREEBOG

Yet another implementation of GOST-34.11-2018 (unofficially called "STREEBOG") - a Russian cryptographic hashing function (from now on called "The Standard" in documentation and code). It is packaged as a library for easy of reuse.

## Justification

*There are a lot of different implementations of this algorithm out there. Why making another one?*

The main justification for creation of this project was its educational value. I learned a lot about cryptography as a whole and this algorithm in particular while working on it.

## Implementations

There are different implementations of the function. For now there are two:

* Reference implementation (path: src/lib/reference, -DLIBGOST34112018=REFERENCE flag CMake) - a reference implementation which follows The Standard as closely as possible. It is very for educational purposes. It is recommended to study this version first, and then moving on to the optimized implementation. It is also recommended for one to have read The Standard _before_ reading the code.

* Optimized implementation (path: src/lib/optimized, -DLIBGOST34112018=OPTIMIZED flag CMake) - a more optimized implementation that uses lookup-tables to accelerate some computations.

## Building
### Dependencies

Only libc is needed.

### Commands

This project uses CMake as a build system. To build it, this set of commands in your terminal:

```
# for optimized implementation
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DLIBGOST34112018_TYPE=OPTIMIZED .. && cmake --build .

# for reference implementation
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DLIBGOST34112018_TYPE=REFERENCE .. && cmake --build .
```

## Why does the code have such weird variable and function names?

TLDR: To keep uniformity of naming between The Standard and the code.

Since The Standard contains primarily mathematical foundations of the algorithm, it is written in a very formal and concise mathematical language. To prevent confusion caused by difference in names of variables and functions in code and in The Standard, the decision has been made to keep names as close to The Standard as possible.

## Examples

To see how to use this library (there is only one function you need to know, really), look no further than src/test/test.c file. It contains tests which serve as examples of using this library.

## License

SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later
