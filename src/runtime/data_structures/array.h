#pragma once
#include <any.h>
#include <vector>

class Array : public Basic_obj {
private:
    std::vector<Any> data;
public:

    /**@brief empty list [] */
    Array();
    Array(std::initializer_list<Any> l);
    Array(int64_t size, Any value);

    static Tag get_tag();

    void append(Any x);
    void append(int64_t x);
    void append(double x);

    Any operator[](int64_t i);
    void *operator new(size_t size);


    friend int64_t len(Array x);
};

