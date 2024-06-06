#include <string>
#include <cstdint>
#include <functional>
#include <utility>
#include "obj.h"

#pragma once

static constexpr uint64_t BIAS         =    0x1000000000000uLL;
static constexpr uint64_t INT_TAG      = 0xFFFC000000000000uLL;

// forward declaration of Array
class Array;

union Any {
    uint64_t integer;
    double real;
public:
    Any ();

    /**@brief OCCUPY: 0xFFFC - 0xFFFF
     * @pre x is in 50-bits complement (top 15 bits are all 0's or 1's)
     */
    Any (int64_t x);
    Any (int x);

    /**@brief OCCUPY: 0x0001 - 0xFFF9
     * @pre x is a naturally occurring double
     */
    Any (double x);

    /**@brief OCCUPY: 0x0000 */
    Any (void* x);

    Any (const Basic_obj& x);

    /**@brief OCCUPY: 0xFFFA
     * @pre x.length () <= 5
     */
    Any (std::string x);
    // when initializing an array from its constructor
    // I'm not sure if we should use Array x instead of Array&& x
    Any (Array&& x);

    Any& operator=(int64_t x);
    Any& operator=(int x);
    Any& operator=(double x);
    Any& operator=(void* x);
    Any& operator=(Array&& x);

    operator int64_t();

    Any& operator[] (int64_t i);
    Any operator[] (int64_t i) const; // return-by-value is faster

    void append(Any x);
    void append(int64_t x);
    void append(double x);
};

// enum type?

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
Array to_array(Any x);

// as_* functions are used in compiled code to ensure that the type
// matches the expected type. as_* calls are generated from source
// code that looks like expression.as_int
int64_t as_int(Any x);
double as_real(Any x);
void *as_ptr(Any x);

/** @brief Obtains the underlying type of an Any, mainly for debugging */
std::string get_type(Any x);









/** --------- IMPLEMENTATION ---------------- */

inline Any::Any(int64_t x) {
#ifdef DEBUG
    int64_t tmp = x & 0xFFFE000000000000;
    if (tmp != 0 && tmp != 0xFFFE000000000000) {
        throw std::runtime_error("Precondition failed: integer value corrupted:");
    }
#endif
    integer = static_cast<uint64_t>(x) | INT_TAG;
}

inline Any::Any(int x) {
    integer = static_cast<uint64_t>(x) | INT_TAG;
}

inline Any::Any(double x) {
    real = x;
#ifdef DEBUG
    if ((integer & 0x7FF0000000000000) == DOUBLE_UP) {
        throw std::runtime_error("Precondition failed: Nan or Inf value.");
        return {};
    }
#endif
    integer += BIAS;
}

inline Any::Any(void* x) {
    // not integer = reinterpret_cast<uint64_t>(x);?
    integer = *reinterpret_cast<uint64_t*>(&x);
#ifdef DEBUG
    if (integer & TAG_MASK) {
        std::cerr << "Precondition failed: pointer corrupted" << std::endl;
        return {};
    }
#endif
}
