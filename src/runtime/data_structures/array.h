#pragma once
#include <vector>
#include <limits>

class Array;


class Array : public Heap_obj {
public:
    // vector takes 24 bytes, so we need 3 slots. Heap_obj has 1, so
    // we need 2 more. This expression is essentially roundup(24 / 8) - 1:
    int64_t more_slots[((sizeof(std::vector<Any>) + sizeof(int64_t) - 1) /
                        sizeof(int64_t)) - 1];
    Array();
    Array(std::initializer_list<Any> l);
    Array(int64_t size, Any value);
    Array(Array *x);
    // int64_t available_len();
    // void expand();
    Array *append(Any x);
    Array *append(int64_t x);
    Array *append(double x);
    Any last();
    Array *insert(int64_t i, Any x);
    Any unappend();
    Array *uninsert(int64_t i, int64_t j = std::numeric_limits<int64_t>::max());
    Array *reverse();
    Array *copy();
    Array *set_len(int64_t new_len);


    // to store at an address, use this:
    void set(int64_t index, Any value);
    void set(int64_t index, double value);
    void set(int64_t index, int64_t value);

    // important: use brackets only for read
    // Any& operator[](int64_t i);
    Any operator[](int64_t i) const;
    int64_t len() const;

    Any call(Any method, Array *args, Dict *kwargs);
    friend std::ostream& operator<<(std::ostream& os, Array *x);
    friend std::string debug_str(Array *x);
protected:
    std::vector<Any> * get_vector() const;

};

int64_t len(Array *x);

Array *subseq(Array *x, int64_t start, int64_t end = std::numeric_limits<int64_t>::max());
bool is_equal(Array *lhs, Array *rhs);

inline Array empty_array {};

struct ArrayPtr {
    Array* ptr;

    ArrayPtr() : ptr(nullptr) {}
    ArrayPtr(Array* p) : ptr(p) {}
    operator Array*() const { return ptr; }
};

ArrayPtr operator+(ArrayPtr lhs, ArrayPtr rhs);

bool operator==(ArrayPtr lhs, ArrayPtr rhs);

std::ostream& operator<<(std::ostream& os, ArrayPtr x);