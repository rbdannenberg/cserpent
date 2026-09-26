# Objects

Here, objects refers to user-defined CSerpent objects.

## Methods

We have a choice of using ordinary C++ method invocation to call
methods or create our own mechanism. Any Serpent method can be
overridden or inherited, so if we use the ordinary C++ mechanism,
every object will have a vtable pointer and method invocation will
use indirection through the vtable to find the right method.

But we also have to have CSerpent class objects with more complete
descriptions of the class, and every object has a pointer to the
class object, so there is already some overhead for class information.
Rather than writing my_obj.some_method(), which would require a
vtable, we can use my_obj.get_class_ptr()->... to access a per-class
data structure with methods for that class.

In the implementation, we do both, where the normal C++ mechanism is
used when we know the class of the object and the method name, so we
can construct a correct parameter list directly. If we do not know the
class of the object or if the method is a variable (as in the "send"
and "sendapply" functions), we must make a more generic call the way
Serpent would do it, putting positional parameters into an array and
keyword parameters into a dictionary. Then we do a hash table lookup
to find the method, which is a function pointer taking parameters:
(Obj* self, Array *args, const Dict& kwargs). This function will check
parameter types and number and make a C++ style invocation of the
method. Thus we need both the vtable for fast direct calls using type
information and the Cs_class::MemberTable for invoking methods with
dynamic typing.

## Allocation and Garbage Collection

The need for a vtable complicates allocation. In Serpent, we have a
64-bit header at the base of every heap object, but C++ wants to put a
vtable at the base, so there is a conflict. Our solution is to define
class Header to contain only header, and when we need n bytes for
an Object, we allocate sizeof(Header) + n bytes and return the address
that *follows* the Header. The garbage collector will subtract the
size of Header from every pointer to get to the header field.

For non-Object heap objects, we define Heap\_obj to contain 1 slot
named slots, and every heap object (Array, Dict, etc.) inherits
Heap\_obj and uses its methods to access and set slot data.

For Object (the superclass for all user-defined classes), we duplicate
the code in Heap\_obj, except here the slots member will be at an
offset of 8 bytes after the Compiler-created vtable pointer.

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
access function into a .call() function call that takes in args and
keyword args as an Array and Dictionary. (See funcalls.txt).

Note: it seems that variable access will also need a special call when
the class is not known.

# Example Instance Variable (Member Variable)

Suppose we have an instance variable `x` in class `Foo`:
```
class Foo:
    var x
``` 
In C++, this becomes something like:
```
class Foo : public Obj {
public:
    // Obj has member variable slots with only one Any, which is the
    // class pointer. While not strictly valid in the C++ spec, we
    // know any member variable declared here will be placed
    // immediately after slots, so we can allocate as many "more
    slots" as we need. Here, we just need one more slot to hold x:
    Any more_slots[1];  // extends the space allocated for slots
    
    Any get_x() { return slots[1]; }
    Any set_x(Any x) { set_slot(1, x); }  // set_slot needed for GC
};
```

Now supposed we have
```
class Foo:
    int x
``` 
In C++, this becomes something like:
```
class Foo : public Obj {
public:
    // Obj has member variable slots with only one Any, which is the
    // class pointer. While not strictly valid in the C++ spec, we
    // know any member variable declared here will be placed
    // immediately after slots, so we can allocate as many "more
    slots" as we need. Here, we just need one more slot to hold x:
    Any more_slots[1];  // extends the space allocated for slots
    
    int get_x() { return slots[1].integer; }  // no type check needed

    // here, set_slot is NOT needed for GC because x cannot point to
    // an object on the heap, so no special GC handling is needed
    int set_x(int x) { slots[1].integer = x; }
};
```

## Local Variables
Local variables exist for the lifetime of a function invocation, from
the point of the call to the point of the return.

There are no references to local variables, so this gives the compiler
some flexibility.

However, since locals can hold references to objects, the GC must have
access to them.

The solution is that each stack frame must be known and searchable by
the GC for pointers. Thus, we use a stack frame structure containing
all local variables that could hold pointers.  Any other locals,
particularly those declared as int or real, can be declared as
ordinary C++ local variables.

The stack frame is subclassed from `Cs_frame`. Here's an example. In
Serpent, we have:
```
def foo():
    var x
    var y
    int i
    
    i = 3
    x = i + 1
    y = x
    return y
```
And in C++, we get
```
Any foo() {
    struct Frame : public Cs\_frame {
        Any x;
        Any y;
    } L;
    constexpr int sl\_x = 0;  // offsets of locals in L
    constexpr int sl\_y = 1;
    memset(&L, 0, sizeof(L));  // initialize all local pointers to nil
    STD\_FUNCTION\_ENTRY(L, 2);  // make L visible to GC
    i = 3;  // ordinary C++ assignment
    L.set(sl\_x, i + 1);  // x = i + 1
    L.set(sl\_y, L.x);  // y = x  (note local access is just L.x)
    STD\_FUNCTION\_EXIT(L, L.y);  // unlink L from GC's reachable frames
}
```
Note that this is an untested sketch. One thing we omitted is
parameter passing. In Serpent, parameters are treated as local
variables, so probably we can just have the compiler copy parameters
into locals (or in the case of at least int and real, just access them
directly.)

## Global Variables

Globals need to be accessible to GC, but globals in Serpent are just
values associated with symbols, which have to be stored for lookup in
a global symbol dictionary. By making the dictionary visible to GC,
the GC can reach all the global variables.

The problem with globals is we need to have direct access to them
(avoiding a dictionary lookup) so that compiled code can read/write
globals efficiently. On the other hand, these directly-accessible
locations must be tied into the global symbol dictionary.

Two solutions are:
  - symbols, rather than holding symbol values directly as in Serpent's
    runtime, can hold _pointers_ to global variables, where the values
    are stored. These can then be declared as C++ globals.
  - globals can be represented as _pointers_ to symbols where values
    are stored.
    
The first solution seems simplest, so a global declaration will be something like:
```
Any global_x;
```
and then in some initialization routine that runs before "main":
```
    define\_global("x", &global\_x, CS_ANY);
```
which will find "x" in the symbol table (or enter it), and store
the address of `global_x` and its type.

Access to `Any` type globals is simple: you can write `global_x` to
get the value but you need to call a function when storing to tell the
GC that the stored object is (still) reachable. What to do depends on
the type of the global. For `Any`, you call:
```
    set\_any\_global(&global_x, value);
```
For storing an `Any` into a global declared to be of a particular
class, you need to check the type:
```
    assert(isinstance(value, class\_of\_global);
    PTR_WRITE_BLOCK(value);
    global\_x = value;
// where PTR_WRITE_BLOCK is really IF_HEAP_MAKE_GRAY -- see definition
```
To implement `set\_symbol\_value('x', value)`, we do a symbol lookup
of 'x' which gives us a symbol. The symbol _could_ have the address of
a global variable, in which case we use the type information stored in
the symbol to check for type compatibility. Then we store the value at
the address.

It is possible to create a new symbol, e.g. `intern('y')` that has no
associated global. In that case, the type information should indicate
that the symbol object maintains a value of type Any (rather than a
pointer to a C++ global), and we can set the value slot of the symbol
directly to implement `set\_symbol\_value('y', value)`.

## Class, Class Class, Object Class

Classes are represented by objects that store information about the
class such as the slot layout and superclass. Therefore, classes
themselves are of C++ class Obj, but in CSerpent, if `Foo` is a class,
then `object_class(Foo)` fails: classes are not objects.

To create a class, we make a Cs\_class object and set it as the value
of a symbol. The Cs\_class is a Cs\_Obj, but the class pointer is nil.
The class name is stored in slot 1 (not true in Serpent, where classes
are anonymous, but in practice, it's nice to assume a 1:1 relationship
between Class names and the Class itself.) The superclass is stored in
slot 5.

To create an instance of a class, we use the class to find the number
of slots and allocate a Heap_obj. We set the class pointer to the
class. The class then tells the GC how many slots and where to find
possible pointers.

### Obj Slots 

slot 0: class pointer

### Class Slots

slot 0: pointer to superclass  
slot 1: class name (== css\_Class)
slot 2: number of instance slots (== 6)
slot 3: bit map of instance slots of type Any (or pointer)
slot 4: MemberTable pointer



