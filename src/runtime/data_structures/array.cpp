// array.cpp - implementation of cserpent Array type

#include "any.h"
#include "gc.h"
#include "obj.h"
#include "array.h"
#include "csmem.h"
#include "op_overload.h"
#include "dictionary.h"
#include <algorithm>
#include <sstream>

static std::vector<Any> *to_vector(const Array& x) {
    return (std::vector<Any> *) x.slots;
}

Array::Array() {
    set_tag(tag_array);
    // use placement new to create vector at &slots[0]:
    new(slots) (std::vector<Any>);
}

Array::Array(std::initializer_list<Any> l) {
    set_tag(tag_array);
    new (slots) std::vector<Any> {l};
}

Array::Array(int64_t size, Any value) {
    set_tag(tag_array);
    std::vector<Any> &data = *new(slots) (std::vector<Any>)(size);
    if (size > 0) {
        set(0, value);  // handles color and write_block for GC
        for (int64_t i = 1; i < size; i++) {  // fill the rest with value
            data[i] = value;
        }
    }
}

std::vector<Any> * Array::get_vector() const {
    return (std::vector<Any> *) slots;
}

Array::Array(const Array& x) {
    set_tag(tag_array);
    new(slots) std::vector<Any> {*(x.get_vector())};
}

static int64_t handle_index(int64_t i, int64_t len) {
    if (i < 0) {
        i = len + i;
    }
    if (i < 0 || i > len) {
        throw std::out_of_range("Array index out of range");
    }
    return i;
}


Array& Array::append(Any x) {
    std::vector<Any> *data = get_vector();
    data->push_back(nullptr);
    set(data->size() - 1, x);
    return *this;
}


Array& Array::append(int64_t x) {
    return append(Any {x});
}


Array& Array::append(double x) {
    return append(Any {x});
}


void Array::set(int64_t index, Any value) {
    std::vector<Any> *data = get_vector();
    Basic_obj *vptr;
    if (write_block && is_ptr(value) && get_color() != GC_BLACK) {
        basic_obj_make_gray(to_ptr(value));
    }
    data->at(index) = value;
}    


void Array::set(int64_t index, double value) {
    std::vector<Any> *data = get_vector();
    data->at(index) = value;
}


void Array::set(int64_t index, int64_t value) {
    std::vector<Any> *data = get_vector();
    data->at(index) = value;
}


Any& Array::operator[](int64_t i) {
    std::vector<Any> *data = get_vector();
    return data->at(i);
}


Any Array::operator[](int64_t i) const {
    std::vector<Any> *data = get_vector();
    return data->at(i);
}

// Refactor to constexpr function, after validating correctness.
Any Array::call(const Symbol& method, const Array& args, const Dictionary& kwargs) {
    std::string method_str = temp_str(method);
    if (method_str == "append") { // potential symbol optimization here
        check_dispatch("append", args, kwargs, 1, 0);
        return append(args[0]);
    }
    else if (method_str == "last") {
        check_dispatch("last", args, kwargs, 0, 0);
        return last();
    }
    else if (method_str == "insert") {
        check_dispatch("insert", args, kwargs, 2, 0);
        return insert(to_int(args[0]), args[1]);
    }
    else if (method_str == "unappend") {
        check_dispatch("unappend", args, kwargs, 0, 0);
        return unappend();
    }
    else if (method_str == "uninsert") {
        check_dispatch("uninsert", args, kwargs, 2, 0);
        if (args.len() == 1) {
            return uninsert(to_int(args[0]));
        }
        return uninsert(to_int(args[0]), to_int(args[1]));
    }
    else if (method_str == "reverse") {
        check_dispatch("reverse", args, kwargs, 0, 0);
        return reverse();
    }
    else if (method_str == "copy") {
        check_dispatch("copy", args, kwargs, 0, 0);
        return copy();
    }
    else if (method_str == "set_len") {
        check_dispatch("set_len", args, kwargs, 1, 0);
        return set_len(to_int(args[0]));
    }
    throw std::runtime_error("Array: no such method");
}

int64_t Array::len() const {
    std::vector<Any> *data = (std::vector<Any> *) slots;
    return data->size();
}

Array& subseq(const Array& arr, int64_t start, int64_t end) {
    int64_t arr_len = arr.len();
    if (end == std::numeric_limits<int64_t>::max()) {
        end = arr_len;
    }
    if (end < 0) {
        end = arr_len + end;
    }
    if (start < 0) {
        start = arr_len + start;
    }
    // do bounds checking: 0 <= start <= end <= arr_len
    if (start < 0 || start > end || end > arr_len) {
        throw std::out_of_range("subseq: out of range");
    }
    auto result = new Array {};
    for (int64_t i = start; i < end; i++) {
        result->append(arr[i]);
    }
    return *result;
}

bool is_equal(const Array& lhs, const Array& rhs) {
    return &lhs == &rhs;
}

std::ostream &operator<<(std::ostream &os, const Array &x) {
    std::vector<Any> &data = *(x.get_vector());
    os << "[";
    for (auto i = data.begin(); i != data.end(); i++) {
        os << *i;
        if (i != data.end() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

std::string debug_str(const Array& x) {
    std::vector<Any> &data = *(x.get_vector());
    std::stringstream ss;
    ss << "[";
    for (auto i = data.begin(); i != data.end(); i++) {
        ss << *i;
        if (i != data.end() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();

}


Any Array::last() {
    std::vector<Any> *data = get_vector();
    return data->back();
}

Array& Array::insert(int64_t i, Any x) {
    std::vector<Any> *data = get_vector();
    // do bounds checking
    i = handle_index(i, data->size());
    data->insert(data->begin() + i, x);
    return *this;
}

Any Array::unappend() {
    std::vector<Any> *data = get_vector();
    Any last = data->back();
    data->pop_back();
    return last;
}

Array& Array::uninsert(int64_t i, int64_t j) {
    std::vector<Any> *data = get_vector();
    i = handle_index(i, data->size());
    if (j == std::numeric_limits<int64_t>::max()) {
        data->erase(data->begin() + i);
    } else {
        j = handle_index(j, data->size());
        data->erase(data->begin() + i, data->begin() + j);
    }
    return *this;
}

Array& Array::reverse() {
    std::vector<Any> *data = get_vector();
    std::reverse(data->begin(), data->end());
    return *this;
}

Array& Array::copy() {
    return *(new Array (*this));
}

Array& Array::set_len(int64_t new_len) {
    std::vector<Any> *data = get_vector();
    data->resize(new_len);
    return *this;
}
