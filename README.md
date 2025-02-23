# GOST-34112018-STREEBOG
Yet another implementation of GOST-34.11-2018 (unofficially called "STREEBOG") - a Russian cryptographic hashing function. It is packaged as a library for easy reuse.

# Building
This project uses CMake as a build system. To build it run this set of commands in your terminal:

```
mkdir build && cd build && cmake .. && cmake --build .
```

# Examples
To see how to use this library (there is only one function you need to know, really), look no further than src/test/test.c file. It contains tests which serve as examples of using this library.