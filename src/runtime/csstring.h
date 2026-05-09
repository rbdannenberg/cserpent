//
// Created by anthony on 7/2/24.
//

// A string is represented by a heap object String, which holds
// the address of a C++ std::string. We need this level of
// indirection (heap object points to string object which wraps
// a pointer to actual string data) instead of simply using string
// because string uses smart-pointer techniques, so it's not
// simply a one-word pointer. Also, it must participate in our
// garbage collection world, where essentially everything is in
// the heap. Looked at another way, a string is an "extern"
// which we represent as a pointer wrapped in a heap object.
//
// Heap objects can be anything, but we'd like to be able to
// declare a value as "string" in CSerpent rather than "var"
// which gives us a dynamically-typed Any. In C++, we
// represent pointers known to point to a String on the heap
// as StringPtr, a struct that simply contains a pointer to
// the heap. A StringPtr has zero high-order bits, so it can
// be aliased with Any. The only difference between StringPtr
// and Any is that Any is a word that can store lots of types,
// while a StringPtr is a word that can only point to a String
// on the heap. A StringPtr cannot hold a nullptr.

#pragma once
#include <string>
#include <cstdint>
#include <limits>
#include <iostream>
#include "heap_obj.h"

class String : public Heap_obj {
public:
    String(const std::string &s);
    String(const char *s = NULL);
    std::string *get_string() const {
        return reinterpret_cast<std::string *>(slots[0].integer); }
    const char *get_c_str() const {
        return reinterpret_cast<std::string *>(slots[0].integer)->c_str(); }
    int64_t len() const {
        return reinterpret_cast<std::string *>(slots[0].integer)->size(); }
};

std::ostream& operator<<(std::ostream& os, const String& x);

std::ostream& operator<<(std::ostream& os, String* x);

String operator+(const String& lhs, const String& rhs);

bool operator==(const String& lhs, const String& rhs);

int64_t len(String *x);

struct StringPtr {
    String* ptr;

    StringPtr() : ptr(nullptr) {}
    StringPtr(String* p) : ptr(p) {}
    operator String*() const { return ptr; }
};

StringPtr operator+(StringPtr lhs, StringPtr rhs);

bool operator==(StringPtr lhs, StringPtr rhs);

std::ostream& operator<<(std::ostream& os, StringPtr x);

int64_t find(StringPtr s, StringPtr pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(StringPtr s, const char *pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(const char *s, StringPtr pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

int64_t find(const char *s, const char *pattern, int64_t start = 0,
             int64_t end = std::numeric_limits<int64_t>::max());

StringPtr subseq(StringPtr s, int64_t start, int64_t end = std::numeric_limits<int64_t>::max());

StringPtr toupper(StringPtr s);
StringPtr tolower(StringPtr s);
