#include <string>
#include <cstdint>
#include <functional>
#include <utility>

#pragma once

// forward declaration of Array and Dictionary for call()

class String;
class Array;
class Symbol;
class Dictionary;
class Basic_obj;
class Obj;

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
    Any(std::string x); // might need to change this to const reference
    Any(const Array& x);
    Any(const Dictionary& x);
    Any(const Obj& x);

    /// Implicit conversion: assignment
    /// Return type allows for chaining
    Any& operator=(int64_t x);
    Any& operator=(int x);
    Any& operator=(double x);
    Any& operator=(void* x);
    Any& operator=(const Array& x);
    Any& operator=(const Dictionary& x);
    Any& operator=(const Obj& x);

    /// Implicit conversion: type-cast operator
    operator int64_t();
    operator double();

    Any& operator[](int64_t i);
    Any operator[](int64_t i) const; // return-by-value is faster

    void append(Any x);
    void append(int64_t x);
    void append(double x);

    Any call(const std::string& method, const Array& args, const Dictionary& kwargs);
    Any& get(const std::string& member);
};

// should we consider creating an enum type for the various underlying types?

bool is_int(Any x);
bool is_real(Any x);
bool is_ptr(Any x);
bool is_shortstr(Any x);
bool is_symbol(Any x);

// these to_* functions do not check that the underlying type is correct
// for that, use as_* functions
int64_t to_int(Any x);
double to_real(Any x);
Basic_obj *to_ptr(Any x);
std::string to_shortstr(Any x);
Array& to_array(Any x);

// as_* functions are used in compiled code to ensure that the type
// matches the expected type. as_* calls are generated from source
// code that looks like expression.as_int
int64_t as_int(Any x);
double as_real(Any x);
void *as_ptr(Any x);

/// Obtains the underlying type of an Any, mainly for debugging.
std::string get_type(Any x);
