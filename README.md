# GOST-34112018-STREEBOG
Yet another implementation of GOST-34.11-2018 (unofficially called "STREEBOG") - a Russian cryptographic hashing function (from now on called "The Standard" in documentation and code). It is packaged as a library for easy of reuse.

## Variations
There are different variations of the implementation of the function. For now there are two:
* Reference implementation (path: ./reference) - a reference implementation which follows The Standard as closely as possible.
* Optimized implementation (path: ./optimized) - a more optimized implementation that uses lookup-tables to accelerate some computations.

## Building
This project uses CMake as a build system. To build it, cd into the directory with the desired variation and run this set of commands in your terminal:

```
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

## Why does the code have such weird variable and function names?
TLDR: To keep uniformity of naming between The Standard and the code.

Since The Standard contains primarily mathematical foundations of the algorithm, it is written in a very formal and concise mathematical language. To prevent confusion caused by difference in names of variables and functions in code and in The Standard, the decision has been made to keep names as close to The Standard as possible.

## Examples
To see how to use this library (there is only one function you need to know, really), look no further than src/test/test.c file. It contains tests which serve as examples of using this library.

## License
SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later
