# zoocpp
Elaboration zoo in C++

See:
    - https://github.com/AndrasKovacs/elaboration-zoo (Haskell)
    - https://github.com/smimram/ocaml-elaboration-zoo (Ocaml)

What is working:
    - implemented up to step 3
    - parser for step 4

What is not working:
    - I can't figure out how to parse named implicit arguments
        - temporary fix: the syntax is {! X = Term } to tell the parser explicitly about them
