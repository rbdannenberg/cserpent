Overall goal currently:
Get the full pipeline working for all tests in ``tests/serpent/full_pipeline``.

Change symbol table to use a CSerpent dictionary for simpler GC of globals.


## Backend/Code Generation
====================
- Bottom-up type inference that affects code generation.
- Functiom calls

====================
Testing/Benchmarks
====================

- Gather more benchmarks: string manipulation, dictionary lookup, dynamic dispatch through classes.
- Gather more Serpent tests: See ``tests/serpent/full_pipeline/omitted.txt`` for more information.
- Get all tests in ``tests/serpent/full_pipeline`` to pass.
- Write scripts to run benchmarks and write to ``benchmarks/benchmarks.csv`` in one go.
- Write scripts to run tests in one go.
