CSerpent is an experimental compiler and runtime designed to compile Serpent programs (with slight changes to add minimal type declarations). A minimal proof-of-concept study suggests speedup of 25x (suggesting roughly 4x slower than C++) is possible without losing the character of Serpent (Python like) programming. See [Making Python Run Fast](https://www.cs.cmu.edu/~rbd/blog/fast/fast-blog10jan2024.html) for more information.

For contributors, see TODO.md and FILE_STRUCTURE.md.

## Basic testing:
To compile a cserpent program:
`    serpent64 compiler.srp file-to-compile.srp -o test_temp -c -d`
The -c flag will create CMakeLists.txt and compile.
The -d flag will run with more debug output.

To (re)compile the generated C++ code in test_temp:
```
    cd test_temp
    cmake --build build
```
### Example:
```
se compiler.srp tests/serpent/simple/oneplustwo.srp -o test_temp -c -d
```
compiles and runs oneplustwo.srp, printing the output "3".

## Tests:

./run_tests.sh automates the testing process.

Place any serpent file in the tests/serpent/full_pipeline directory
and the script will tell us whether both serpent and cserpent output
the same result to stdout.
