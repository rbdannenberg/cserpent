# Globals


Global variables in Serpent are translated to globals in C++.

If a global is of type "Any" (In Cserpent, either a var or
undeclared), it might contain a value that is on the heap, in
which case the GC must know about this value. Also, Serpent has
the function symbol_set_value(symbol, value), which means it
must be possible to find where the symbol's value is stored
at run time. In Serpent, global values are stored in the
symbol itself. Symbols have slots for the symbol name, global
value, and function value (since a symbol can denote both a
global and a function - functions are not values).

In CSerpent, we solve the problem by giving symbols the *address* of
the C++ global value where the value is stored. This allows the C++
compiler to read and write globals efficiently from compile-time
addresses. At the same time, since all symbols can be reached through
a global symbol table, the GC can trace through them all to find all
references to the heap.

A special problem with CSerpent is that globals can be typed. To
support symbol_set_value(), each symbol contains not only the address
of the global but the type. Runtime type checks in symbol_set_value()
can implement type checking and coercion using the same rules used by
the compiler for assignments.

## Assignment

Assignment of a value to a global must coordinate with garbage
collection (GC). If the value is a heap object, it is obviously
reachable, so it must be marked (made GRAY) when the GC write_block
variable is true. The condition is: value is a non-null heap object
pointer, and value's color is BLACK. For other write-block cases where
we store into another heap object, we also require that the object
being modified is non-BLACK. (If it is, it is obviously reachable, so
later, GC will get to it, making it GRAY and eventually making the
value GRAY.)

