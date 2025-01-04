# zoocpp
Elaboration zoo in C++

## References
- https://github.com/AndrasKovacs/elaboration-zoo (Haskell)
- https://github.com/smimram/ocaml-elaboration-zoo (Ocaml)

## Requirements
- cmake
- flex
- bison

## How to run

The first time:

    mkdir build && cd build

The first time, everytime you modify the CMakeLists.txt files, when you remove files from `build`, 
or when you want to change the build type:

    cmake -S .. -B . -DCMAKE_BUILD_TYPE=<build_type>

`<build_type>` can be:
- `Debug` if you want to modify the source, to get access to debug logs;
- `Release` when you are only interested in running examples and tests.

Everytime you change a source file:

    make

To run tests:

    ctest

If the tests are ok, you would get something like the following:

    Test project <path_to_build>
        Start 1: PositiveTests
    1/2 Test #1: PositiveTests ....................   Passed    <some_time>
        Start 2: NegativeTests
    2/2 Test #2: NegativeTests ....................   Passed    <some_time>

    100% tests passed, 0 tests failed out of 2

    Total Test time (real) =   <some_time>

To run on a particular file:

    ./bin/ElaborationZoo -f <path_to_file>

You can also run

    ./bin/ElaborationZoo

without options and write your terms in the standard input.

## What is working:
- implemented up to step 5
    - de Bruijn indices and levels
    - holes
    - implicit args and lambdas
    - pruning
    - first class polymorphism
- Google-test suite

# What is not working:
- In the example `pos6.txt`, elaboration fails on `sym`, while it was working before the introduction of first class polymorphism.
I do not know if this is an expected failure. Fix: added more annotations.
- I can't figure out how to parse named implicit arguments properly
    - temporary fix: the syntax is {! X = Term } to tell the parser explicitly about them
- When building, you may get a warning about a comparison of integers of different signs from the .cc generated file by flex. This happens at least with `clang` on MacOS. This is harmless.

# TODOS:
- Real errors
- a better postponment strategy?
- cleaning up the usage of C++ polymorphism
- ...
