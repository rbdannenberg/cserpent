#pragma once
#include <any.h>
#include <vector>


class Array_heap : public Basic_obj {
private:
    std::vector<Any> data;
    /**@brief empty list [] */
    Array_heap();
    Array_heap(std::initializer_list<Any> l);
    Array_heap(int64_t size, Any value);
public:
    static Tag get_tag();

    friend int64_t len(Array x);

};

class Array {
public:
    Array_heap *ptr;
    Array();
    Array(std::initializer_list<Any> l);
    Array(int64_t size, Any value);

    void append(Any x);
    void append(int64_t x);
    void append(double x);

    Any operator[](int64_t i);
    friend int64_t len(Array x);

};

