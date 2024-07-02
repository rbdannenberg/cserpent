#pragma once
#include "any.h"
#include <vector>

class Array;

// Array is a header for Array_data, which stores data
// The object contains:
//     slots[0] - Array length
//     slots[1] - Array_data
class Array : public Basic_obj {
public:
    // vector takes 24 bytes, so we need 3 slots. Basic_obj has 1, so
    // we need 2 more. This expression is essentially roundup(24 / 8) - 1:
    int64_t more_slots[((sizeof(std::vector<Any>) + sizeof(int64_t) - 1) /
                        sizeof(int64_t)) - 1];
    Array();
    // explicit Array(Array_heap *hptr); // make this private?
    // Array(std::initializer_list<Any> l);
    Array(int64_t size, Any value);
    // int64_t available_len();
    // void expand();
    void append(Any x);
    void append(int64_t x);
    void append(double x);
    // to store at an address, use this:
    void set(int64_t index, Any value);
    void set(int64_t index, double value);
    void set(int64_t index, int64_t value);

    // important: use brackets for read only
    Any& operator[](int64_t i);
    Any operator[](int64_t i) const;
    friend int64_t len(Array x);
};


/*
class Array_data : public Basic_obj {
  public:
};
*/
