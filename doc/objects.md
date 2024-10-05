# Objects

Here, objects refers to user-defined CSerpent objects.

## Object Instance Variables (aka Member Variables)

Because of garbage collection reasons, C++ objects will not have
member variables. Instead, these will be stored in a slots[] array,
which store Any's. The backend will compile getter and setter
functions for each member variable, which will access the slot[]
array.

Thus, the only outward-facing members for classes will be
functions. This will be accomplished by the backend walking the AST
and recursively transforming member variable access to member function
calls.

When accessing members of an object whose class is unknown, there will
be one further transformation - which is to transform every member
function call into a .call() function call that takes in args and
kwargs as an Array and Dictionary. (See funcalls.txt).

Note: it seems that variable access will also need a special call when
the class is not known.


