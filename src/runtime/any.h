#include <string>
#include <cstdint>
#include <functional>
#include <utility>
#include <cassert>
// Any should be the root of all includes. Therefore it should not include any other project file.

#pragma once

using std::size_t;

// forward declaration of Array and Dictionary for call()

union String;
class Array;
union Symbol;
class Dictionary;
class Basic_obj;
class Obj;

constexpr uint64_t BIAS         =    0x1000000000000uLL;
constexpr uint64_t INT_TAG      = 0xFFFC000000000000uLL;
constexpr uint64_t TAG_MASK     = 0xFFFF000000000000uLL;
constexpr uint64_t PTR_TAG      = 0x0000000000000000uLL;
constexpr uint64_t STR_TAG      = 0xFFFA000000000000uLL;
constexpr uint64_t SYMBOL_TAG   = 0xFFFB000000000000uLL;


union Any {
    uint64_t integer;
    double real;
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
    Any(String x); // take by value because we don't want aliasing
    Any(Symbol x); // likewise no aliasing wanted
    Any(const Array& x);
    Any(const Dictionary& x);
    Any(const Obj& x);
    Any(bool x);
    Any(const std::ostream& x); ///< until print becomes not a function call

    /// Implicit conversion: assignment
    /// Return type allows for chaining
    Any& operator=(int64_t x);
    Any& operator=(int x);
    Any& operator=(double x);
    Any& operator=(String x);
    Any& operator=(Symbol x);
    Any& operator=(const Array& x);
    Any& operator=(const Dictionary& x);
    Any& operator=(const Obj& x);
    Any& operator=(bool x);

    /// Implicit conversion: type-cast operator
    operator int64_t();
    operator double();
    operator String();
    operator Symbol();
    operator Array&();
    operator bool();


//    Any& operator[](int64_t i);
    void set(int64_t i, Any val); // Use this instead of assignment through [] (gc reasons).
    Any operator[](int64_t i) const; // return-by-value is faster

    void append(Any x);
    void append(int64_t x);
    void append(double x);

    Any call(const Symbol& method, const Array& args, const Dictionary& kwargs);
};

inline const Any nil = Any {nullptr};
inline const Any False = nil;
extern const Any t;
extern const Any True;


enum class Any_type {
    INT,
    REAL,
    STR,
    SYMBOL,
    ARRAY,
    DICT,
    OBJ,
    NIL,
    T
};

// should we consider creating an enum type for the various underlying types?

bool is_int(Any x);
bool is_real(Any x);
bool is_ptr(Any x);
bool is_str(Any x);
bool is_symbol(Any x);
Any_type get_type(Any x);

// these to_* functions do not check that the underlying type is correct
// for that, use as_* functions
int64_t to_int(Any x);
double to_real(Any x);
Basic_obj *to_ptr(Any x);
String to_str(Any x);
Symbol to_symbol(Any x);
Array& to_array(Any x);
Dictionary& to_dict(Any x);

// as_* functions are used in compiled code to ensure that the type
// matches the expected type. as_* calls are generated from source
// code that looks like expression.as_int
int64_t as_int(Any x);
double as_real(Any x);
String as_str(Any x);
void *as_ptr(Any x);

/// Obtains the underlying type of an Any, mainly for debugging.
std::string get_type_str(Any x);
