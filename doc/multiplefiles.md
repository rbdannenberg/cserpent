# Multiple-File Translation

We want CSerpent to compile incrementally. Not everything is
implemented this way yet.

The general structure is that every Serpent file is "executed"
sequentially when it is loaded. To get this behavior from C++, we put
every top-level executable command into a function ending in "_main".
For example, if we compile `foo.srp`, we output variable, class and
function definitions as well as the function `int foo_main()`.

Then, the compiler creates `main.cpp` with function `int main(int
argc, char **argv)`, and within that function calls `foo_main()` and
any other "main" files for included functions, whether they are
included directly or indirectly (using Serpent `require` statements).

We will not support `load` for now, which allows a file to be
loaded/executed multiple times. This might be implemented by to
calling `foo_main()` from `bar_main()` in the case where `bar.srp`
contains `load "foo"`, but it is considered bad practice in Serpent to
use `load` at all, so why support it?

## Global Variables

Globals need to be entered into the symbol table for CSerpent's
runtime and also initialized. We could put global initialization in
`foo_main()` or even in a new function, `foo_globals`, but for now, we
are making a single file, `global_vars.cpp` where *all* globals are
created. There is also `global_vars.h` where *all* globals are
declared. This means that each file need only include `global_vars.h`
rather than a `.h` file for *every* Serpent file's globals.

Just because `global_vars.cpp` changes or even if `global_vars.h`
changes, it may not me necessary to recompile everything because the
actual dependents should be found in the database and non-dependent
files, even though they include the modified `global_vars.h`, need not
be recompiled.

## Incremental Compilation

For simplicity, we would like to rely on CMake to perform incremental
compilation. In this approach, all the compiler needs to do is check
the file dates for `foo.srp` and `foo.cpp` and make sure that of
`foo.cpp` is later. If not, it should recompile `foo.cpp`. But the
problem is that this could change the definitions in `foo.cpp` and
invalidate some other `.cpp` file compiled from another `.srp` file
that "requires" `foo.cpp`.

The solution is to keep track of dependencies. The type of every
function and every variable is maintained in a database for the entire
program. The database also contains every file that depends on that
type declaration. If the type changes, the dependent files are
invalidated and must be recompiled. We can "invalidate" files by
putting something in the database.

Serpent can have circular dependencies. E.g., `foo.srp` can call
`f1()` defined in `bar.srp`, and `bar.srp` can call `f2()` defined in
`foo.srp`. The compiler can guess that `f1()` has type `Any` or even
use the general form of call for untyped functions, but we want to use
the definition of `f1()` to write efficient code, which means it might
take the compiler two passes to get it right.

## Compilation Order

To allow the user to help the compiler some, compilation order should
be the same as the order in Serpent, so the database of declarations
should also include what files are required by each file. Then, when
we compile `foo.srp`, we will first compile any files required by
`foo.srp`, then `foo.srp`. This ordering will proceed depth first,
e.g., if `foo` requires `bar` requires `baz`, we compile `baz` first,
but only if there is a need to recompile `baz`.



