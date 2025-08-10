#include <string>
#include <cstdint>
#include <functional>
#include <utility>
#include <cassert>
// Any should be the root of all includes. Therefore it should not include any other project file.

#pragma once

using std::size_t;

// forward declaration of Array and Dict for call()

class String;
struct StringPtr;
class Array;
struct ArrayPtr;
class Symbol;
class Dict;
class Heap_obj;
class Obj;

constexpr uint64_t BIAS         =    0x1000000000000uLL;
constexpr uint64_t INT_TAG      =  0xFFFC000000000000uLL;
constexpr uint64_t INT_MASK     = ~0xFFFE000000000000uLL;  // 49 bits
constexpr uint64_t TAG_MASK     =  0xFFFF000000000000uLL;
constexpr uint64_t PTR_TAG      =  0x0000000000000000uLL;
constexpr uint64_t SYMBOL_TAG   =  0xFFFB000000000000uLL;
constexpr uint64_t SHORT_TAG    =  0xFFFC000000000000uLL;
constexpr uint64_t STR_TAG      =  0xFFFD000000000000uLL;
// after subtracting BIAS, valid real (double) bits should be less than:
constexpr uint64_t REAL_LIMIT   =  0xFFFA000000000000uLL;


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define LITTLE_ENDIAN_ORDER
#else
#error __BYTE_ORDER__ is undefined
#endif

#ifdef LITTLE_ENDIAN_ORDER
#define SHORTSTR_BASE 0
#else
#define SHORTSTR_BASE 2
#endif



union Any {
    uint64_t integer;
    double real;
    char bytes[8];
public:
    Any();

    /**@brief OCCUPY: 0xFFFC - 0xFFFF
     * @pre x is in 50-bits complement (top 15 bits are all 0's or 1's)
     */
    Any(int64_t x);
    Any(int x);

    /**@brief OCCUPY: 0x0001 - 0xFFF9
     * @pre x is a naturally occurring double
     */
    Any(double x);

    /**@brief OCCUPY: 0x0000 */
    Any(void* x);

    /**@brief OCCUPY: 0xFFFA
     * @pre x.length () <= 5
     */
    Any(String *x); // take by value because we don't want aliasing
    Any(StringPtr x);
    Any(Symbol *x); // likewise no aliasing wanted
    Any(Array *x);
    Any(ArrayPtr x);
    Any(Dict *x);
    Any(Obj *x);
    Any(const char *x);
    Any(std::string &x);
    Any(bool x);
    Any(const std::ostream& x); ///< until print becomes not a function call

    /// Implicit conversion: assignment
    /// Return type allows for chaining
    Any& operator=(int64_t x);
    Any& operator=(int x);
    Any& operator=(double x);
    Any& operator=(String *x);
    Any& operator=(StringPtr x);
    Any& operator=(Symbol *x);
    Any& operator=(Array *x);
    Any& operator=(ArrayPtr x);
    Any& operator=(Dict *x);
    Any& operator=(Obj *x);
    Any& operator=(const char *x);
    Any& operator=(bool x);

    /* Implicit conversion: type-cast operator - see note in Any.cpp
    operator int64_t();
    operator double();
    operator String *();
    operator Symbol *();
    operator Array *();
    operator bool();
     */


//    Any& operator[](int64_t i);
    // Use this instead of assignment through [] (gc reasons):
    void set(int64_t i, Any val);
    Any operator[](int64_t i) const; // return-by-value is faster

    bool is(Any x);
    void append(Any x);
    void append(int64_t x);
    void append(double x);

    Any call(Any method, Array *args, Dict *kwargs);
};

inline const Any nil;

enum class Any_type {
    INT,
    REAL,
    STRING,
    SHORT,
    SYMBOL,
    ARRAY,
    DICT,
    OBJ,
    NIL
};

// should we consider creating an enum type for the various underlying types?

bool is_int(Any x);
bool is_real(Any x);
bool is_heap_obj(Any x);
bool is_str(Any x);     // tests for either short string or String
bool is_string(Any x);  // tests for String
bool is_short(Any x);   // tests for short string
bool is_symbol(Any x);
Any_type get_type(Any x);

 // these to_* functions do not check that the underlying type is correct
// for that, use as_* functions
int64_t to_int(Any x);
double to_real(Any x);
Heap_obj *to_heap_obj(Any x);
String *to_string(Any x);
Symbol *to_symbol(Any x);
Array *to_array(Any x);
Dict *to_dict(Any x);
Obj *to_obj(Any x);

// as_* functions are used in compiled code to ensure that the type
// matches the expected type. as_* calls are generated from source
// code that looks like expression.as_int
int64_t as_int(Any x);
double as_real(Any x);
String as_str(Any x);
Heap_obj *as_heap_obj(Any x);

/// Obtains the underlying type of an Any, mainly for debugging.
std::string get_type_str(Any x);

/// Get a const char * from short or long string
const char *get_c_str(const Any *s, int64_t *len_ptr = NULL);
