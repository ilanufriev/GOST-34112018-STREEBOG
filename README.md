# GOST-34112018-STREEBOG

Yet another implementation of GOST-34.11-2018 (unofficially called "STREEBOG") - a Russian cryptographic hashing function (from now on called "The Standard" in documentation and code). It is packaged as a library for easy of reuse.

## Justification

*There are a lot of different implementations of this algorithm out there. Why make another one?*

The main justification for creation of this project was its educational value. I learned a lot about cryptography as a whole and this algorithm in particular while working on it.

## Implementations

There are different implementations of the function. For now there are two:

* **Reference implementation** (path: src/lib/reference, -DLIBGOST34112018=REFERENCE flag in CMake) - a reference implementation which follows The Standard as closely as possible. It is very good for educational purposes, but not performance. It is recommended to study this version first, and then moving on to the optimized implementation. It is also recommended for one to have read The Standard _before_ reading the code.

* **Optimized implementation** (path: src/lib/optimized, -DLIBGOST34112018=OPTIMIZED flag in CMake) - a more optimized implementation that uses lookup-tables to accelerate some computations.

* **AVX2 implementation** (path: src/lib/avx2, -DLIBGOST34112018=AVX2 flag in CMake) - an optimized implementation that uses lookup-tables and AVX2 intrinsics for some Vec512 operations.

## Building
### Dependencies

Only libc is needed.

### Commands

This project uses CMake as a build system. To build it, this set of commands in your terminal:

```
# for avx2 implementation
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DLIBGOST34112018_TYPE=AVX2 .. && cmake --build .

# for optimized implementation
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DLIBGOST34112018_TYPE=OPTIMIZED .. && cmake --build .

# for reference implementation
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DLIBGOST34112018_TYPE=REFERENCE .. && cmake --build .
```

## Why does the code have such weird variable and function names?

**TLDR:** To keep uniformity of naming between The Standard and the code.

Since The Standard contains primarily mathematical foundations of the algorithm, it is written in a very formal and concise mathematical language. To prevent confusion caused by difference in names of variables and functions in code and in The Standard, the decision has been made to keep names as close to The Standard as possible.

## Examples

To see how to use this library look into src/util/gost34112018_cli.c, which serves as an example of using the library API.

## Command-line tool

**gost34112018_cli** is a simple command-line tool that allows to compute GOST 34.11-2018 hashsum on the fly:

```
$ ./gost34112018_cli --help
Usage: gost34112018_cli [OPTION...] [ARGS...]
My program description.

  -b, --big-endian           Print hash in big endian format (by default hash
                             is printed in little endian format).
  -n, --no-newline           Print hash with no newline character at the end.
  -s, --hash-size=HASH_SIZE  Size of the hash (256 or 512). 512 by default.
  -?, --help                 Give this help list
      --usage                Give a short usage message

$ cat message1.bin | ./gost34112018_cli -s 512 -b
486f64c1917879417fef082b3381a4e211c324f074654c38823a7b76f830ad00fa1fbae42b1285c0352f227524bc9ab16254288dd6863dccd5b9f54a1ad0541b

$ cat message1.bin | ./gost34112018_cli -s 256 -b
00557be5e584fd52a449b16b0251d05d27f94ab76cbaa6da890b59d8ef1e159d
```

## License

SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later
