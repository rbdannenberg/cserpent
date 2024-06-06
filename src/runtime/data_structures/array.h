#pragma once
#include "any.h"
#include <vector>

class Array;

class Array : public Basic_obj {
public:
    std::vector<Any> data;
    Array();
    // explicit Array(Array_heap *hptr); // make this private?
    Array(std::initializer_list<Any> l);
    Array(int64_t size, Any value);

    void append(Any x);
    void append(int64_t x);
    void append(double x);

    Any& operator[](int64_t i);
    Any operator[](int64_t i) const;
    friend int64_t len(Array x);

};

