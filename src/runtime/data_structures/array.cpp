#include "array.h"
#include <csmem.h>

Array::Array() {set_tag(tag_arraydata);}

Array::Array(std::initializer_list<Any> l) : data {l} {set_tag(tag_arraydata);}

Array::Array(int64_t size, Any value) : data (size, value) {set_tag(tag_arraydata);}

void Array::append(Any x) {
    data.push_back(x);
}

void Array::append(int64_t x) {
    data.push_back(Any {x});
}

void Array::append(double x) {
    data.push_back(Any {x});
}

Any& Array::operator[](int64_t i) {
    return data.at(i);
}

Any Array::operator[](int64_t i) const {
    return data.at(i);
}

int64_t len(Array x) {
    return static_cast<int64_t>(x.data.size());
}


