CMake
-----

CMakeLists.txt in this root directory is the only CMakeLists.txt file for this project. It handles builds for 
all C++ code in this proejct, namely runtime, memory management and their respective tests and benchmarks.

CMake targets are:

- ``runtime``: The runtime library. Contains implementation for Any and supporting classes. Almost every executable links to this.
- ``runtime_test``: Tests for the runtime library, such as testing arrays and strings.
- ``runtime_benchmarks``: Benchmarks for the runtime system. Currently benchmarks runtime against native C. Python benchmark needs to be run separately from benchmarks/py/main_benchmark.py.
- ``memtest``: A test for memory management. Compiles but does not currently work.
- ``cpptest``: A test that C++ compiles and runs.

All executables work except for memtest. 

Note: Runtime is part of include directories, so you can include them with #include <any.h> and #include <array.h> etc.

File structure
--------------

- ``src/``: All implementation goes here.
  - ``frontend/``: Implementation for the recursive descent parser in Serpent. Produces an AST defined in ``frontend/ast.srp``.
  - ``backend/``: Static semantic routines and code generation for the AST. Written in Serpent. Walks the AST, producing C++ code.
  - ``runtime/``: Implementation for runtime system in C++ + custom csmalloc and csfree. C++ constructs such as Any are used to implement dynamic typing.
  - ``gc/``: Implementation for garbage collector in C++
  - ``fullstack/``: Prototype implementation of frontend + backend in Python. Deprecated.
- ``tests``: **All** tests go here.
- ``benchmarks``: **All** benchmarks go here.

