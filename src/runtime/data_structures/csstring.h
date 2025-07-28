//
// Created by anthony on 7/2/24.
//

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