#include "array.h"
#include <csmem.h>

Array_heap::Array_heap() : data {} {}

Array_heap::Array_heap(std::initializer_list<Any> l) : data (l) {}

Array_heap::Array_heap(int64_t size, Any value) : data (size, value) {}

Tag Array_heap::get_tag() {
    return tag_array;
}

Array::Array() : ptr {new Array_heap {}} {}

Array::Array(std::initializer_list<Any> l) : ptr {new Array_heap {l}} {}

Array::Array(int64_t size, Any value) : ptr {new Array_heap {size, value}} {}

void Array::append(Any x) {
    ptr->data.push_back(x);
}

void Array::append(int64_t x) {
    ptr->data.push_back(Any {x});
}

void Array::append(double x) {
    ptr->data.push_back(Any {x});
}

Any Array::operator[](int64_t i) {
    return ptr->data[i];
}

int64_t len(Array x) {
    return static_cast<int64_t>(x.ptr->data.size());
}


