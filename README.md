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
        Start 1: ElaborationTests
    1/1 Test #1: ElaborationTests .................   Passed    <test_time>

    100% tests passed, 0 tests failed out of 1

    Total Test time (real) =   <total_time>


## What is working:
- implemented up to step 4
- Google-test suite

# What is not working:
- I can't figure out how to parse named implicit arguments properly
    - temporary fix: the syntax is {! X = Term } to tell the parser explicitly about them
- When building, you may get a warning about a comparison of integers of different signs from the .cc generated file by flex. This happens at least with `clang` on MacOS. This is harmless.
