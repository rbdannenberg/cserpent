#include "array.h"

Array::Array() : data {} {}

Array::Array(std::initializer_list<Any> l) : data {l} {}

Array::Array(int64_t size, Any value) : data {size, value} {}

Tag Array::get_tag() {
    return tag_array;
}

void Array::append(Any x) {
    data.push_back(x);
}

void Array::append(int64_t x) {
    data.push_back(Any {x});
}

void Array::append(double x) {
    data.push_back(Any {x});
}

Any Array::operator[](int64_t i) {
    return data[i];
}

int64_t len(Array x) {
    return static_cast<int64_t>(x.data.size());
}

