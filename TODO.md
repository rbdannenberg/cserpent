Overall goal currently:
Get the full pipeline working for all tests in ``tests/serpent/full_pipeline``.

====================
Frontend/Parsing
====================
- Get working

====================
Backend/Code Generation
====================
- Bottom-up type inference that affects code generation.
- Static semantic routines such as return and intialization checking.

====================
Runtime
====================

- Change dictionary implementation away from multiple inheritance.
- Function, method and operator implementations.
- *Low priority*: Incorporate unicode strings and different endian architectures.

====================
Memory Management
====================

- Get memtest target to work
- Create a test for the garbage collector and test it.
- Bake gc_poll() into overriden new operator.

====================
Testing/Benchmarks
====================

- Gather more benchmarks: string manipulation, dictionary lookup, dynamic dispatch through classes.
- Gather more Serpent tests: See ``tests/serpent/full_pipeline/omitted.txt`` for more information.
- Get all tests in ``tests/serpent/full_pipeline`` to pass.
- Write scripts to run benchmarks and write to ``benchmarks/benchmarks.csv`` in one go.