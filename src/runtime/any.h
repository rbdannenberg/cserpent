#include <string>
#include <cstdint>
#include <functional>
#include <utility>

#pragma once

static constexpr uint64_t BIAS         =    0x1000000000000uLL;
static constexpr uint64_t INT_TAG      = 0xFFFC000000000000uLL;

enum Tag { tag_free,
    tag_symbol,
    tag_integer,
    tag_string,
    tag_array,
    tag_arraydata,
    tag_dict,
    tag_object,
    tag_file };

enum Color { white, grey, black };


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

    // Any (const Basic_obj& x);

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
    const Any& operator[] (int64_t i) const;

    void append(Any x);
    void append(int64_t x);
    void append(double x);
};

// all garbage-collected objects inherit this base class
//
class Basic_obj {
public:
    /* fields in header:
     *
     * header format is:
     *  +---------------------------------------------+
     *  | TAG      | COLOR    | #SLOTS    | NEXT      |
     *  | (5 bits) | (2 bits) | (12 bits) | (45 bits) |
     *  +---------------------------------------------+
     * The NEXT field is a pointer to 8-byte aligned data.
     * It is shifted right 3 bits because the low-order bits
     * are zero. To use the pointer, shift it left 3 bits.
     */
    uint64_t header;   // contains type info, GC color, size
    int64_t slots[1];  // can actually be any number of slots
    
    // on return, Basic_obj header will have #SLOTS initialized
    // and slots[1] will be an unencoded integer slot count if
    // the slot count is greater than 4095, in which case #SLOTS
    // will be zero. The number of slots is based on size which
    // is rounded up to a multiple of 8 (the slot size). The
    // TAG will be tag_object, and the superclass constructors
    // for other types of objects should set TAG accordingly.
    //
    void *operator new(size_t size);

    void operator delete(void *p) noexcept;
    
    Basic_obj() { };

    Tag get_tag(); // replace after implementation
    void set_tag(Tag t);
    bool has_tag(int tag);

    Color get_color();
    void set_color(uint8_t c);
    bool has_color(int tag);

    Basic_obj *get_next();
    void set_next(Basic_obj *ptr);

    int64_t get_size();
    int64_t get_slot_count();
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
Array *to_array(Any x);

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
