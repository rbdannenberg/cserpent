CSerpent is an experimental compiler and runtime designed to compile Serpent programs (with slight changes to add minimal type declarations). A minimal proof-of-concept study suggests speedup of 25x (suggesting roughly 4x slower than C++) is possible without losing the character of Serpent (Python like) programming. See [Making Python Run Fast](https://www.cs.cmu.edu/~rbd/blog/fast/fast-blog10jan2024.html) for more information.

For contributors, see TODO.md and FILE_STRUCTURE.md.

To run tests:
1. Ensure Serpent is installed and in the PATH (`serpent64` in the terminal brings up the interpreter)
2. Clone this repository to the environment variable SERPENTPATH
3. From the cserpent/ top-level directory, run the following command:

`sh run_tests.sh`
4. To compile and execute a single test case, run the following command:

`serpent64 compiler.srp /path/to/cserpent/tests/serpent/full_pipeline/<TESTFILE> -o test_temp -c`

-o flag: directory name where build files are created

-c flag: compile from C++ to an executable

More information about options for compiler.srp can be found in compiler.srp.

(Under the hood, ./run_tests.sh executes the above command and diffs the print output with `serpent64 TESTFILE`)