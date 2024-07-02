Initial implementation of String had default copy construction and assignment, 
with std::string member variable non-const (so copy assignment is possible).

This posed a problem with aliasing/immutability.

Serpent behaviour:
```python
a = "hello"
b = a
a = a + " world"
print(b) # should print "hello"
```

C++ behaviour:
```cpp
String& a = *(new String("hello"));
String& b = a;
a = a + " world"; // or a = toupper(a)
std::cout << b << std::endl; // prints "hello world"
```

Ideally `a = a + " world"` should not modify a, but create a new String object. You might think to modify operator+,
but the issue would still arise if it was replaced by a = toupper(a). The underlying issue is that the copy assignment
preserves the address of a and modifies the object at that address, returning `*this`.

The solution I came up with is to make the std::string member variable const, so that the copy assignment function cannot modify the object.
Instead, it must create a new object on the heap (using new) and return a reference to that object.
This is alright because we know Strings only live on the heap.

```cpp
String& operator=(const String& other) {
// let garbage collector handle the old string
return *(new String {other.str});
}
```

UNFORTUNATELY, it seems the copy assignment will return *this regardless of what we do. 
Given that we want String&s to work interchangably with Anys, I don't see a way to prevent aliasing.
With regards to compiling the above code differently, such as a = copy(a + "world"), the problem is that at we don't know types 
at compile time in general. That's why I only see the solution as overloading a bunch of operators and letting types
resolve at runtime.

(Side note: if the 3rd line was a = *(new String {"goodbye"}) instead, I don't think the copy assignment would get invoked, but TEST THIS.)

SOLUTION: 
- Make string (Serpent) be a String *. This allows the pointer to get reassigned
- As a consequence, indexing would look like (*s)[0]. This is very ugly and tricky to compile.
- Instead, since we have full control over the C++ code, compile this to index(s, 0) instead.
- In general, overloads should only be non-member functions. We will see if this creates issues as we implement.

CAVEATS:
int64_t find(const String &s, const String &pattern, int64_t start=0, int64_t end=std::numeric_limits<int64_t>::max()) {
Oftentimes, pattern is passed as a string literal. This function signature would then implicitly convert it to
a temporary String object that gets automatically cleaned up once the function returns.
int64_t find(const String *s, const String *pattern, int64_t start=0, int64_t end=std::numeric_limits<int64_t>::max()) {
On the other hand, with pointers instead of references, no implicit conversion mechanism exists, so 
1. The compiler would have to wrap the pattern in create_string
2. This causes an additional heap allocation and object thing for the garbage collector to clean up

Non-member operator overloads also don't work if there isn't at least one class or enumeration type :(
