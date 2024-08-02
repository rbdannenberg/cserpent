//
// Created by anthony on 7/7/24.
//

#include <stdexcept>
#include <cassert>
#include <utility>
#include "symbol.h"
#include "array.h"


// distinguish between short and long strings by the 47th bit. Pointers will have 1, short
// strings will have 0, for the nul terminator.
// number of useful characters in a short string (i.e. does not include the nul terminator)
static constexpr int64_t max_short_size = 5;
static constexpr uint16_t string_tag = 0xFFFB;
static constexpr uint64_t long_tag = 0xFFFB800000000000; // for long strings only, 0xFFFA for regular tag, 8 to distinguish between nul terminator
static constexpr uint64_t empty_string = 0xFFFB000000000000;

// O(max_short_size)
static bool is_short_literal(const char * literal) {
    for (int64_t i = 0; i < max_short_size + 1; i++) {
        if (literal[i] == '\0') return true;
    }
    return false;
}

static bool is_short_string(const Symbol& s) {
    return s.chars[5] == '\0';
}

static std::string * get_str_ptr(uint64_t s) {
    return reinterpret_cast<std::string*>(s & ~long_tag); // you might worry about the 47th bit, but that ensures we get a "canonical" pointer
}

static uint64_t get_str_data(std::string * s) {
    return reinterpret_cast<uint64_t>(s) | long_tag;
}



/// len is only defined for Strings, not Symbols, but we still want it as a helper.
static int64_t len(const Symbol& s) {
    if (is_short_string(s)) {
        for (int64_t i = 0; i <= max_short_size; i++) { // len can be anything from 0 to 5
            if (s.chars[i] == '\0') return i;
        }
    } else {
        return get_str_ptr(s.data)->size();
    }
}

/// @brief temporary conversion to exploit the std::string library
/// static so we don't expose it outside of this file
/// I believe std::string has optimizations for short strings as well, so this shouldn't be too wasteful
std::string temp_str(const Symbol& s) {
    if (is_short_string(s)) {
        std::string result {s.chars}; // should stop upon nul terminator
        assert(result.size() == len(s));
        return result;
    } else {
        return *get_str_ptr(s.data);
    }
}

Symbol::Symbol() : data {empty_string} {}
Symbol::Symbol(const char* literal) {
    if (is_short_literal(literal)) {
        for (int64_t i = 0; i < max_short_size; i++) {
            tag = string_tag;
            chars[i] = literal[i];
            chars[5] = '\0';
        }
    } else {
        data = get_str_data(new std::string(literal));
    }
}
Symbol::Symbol(const std::string& s) : Symbol {s.c_str()} {}

Symbol::~Symbol() {
    if (!is_short_string(*this)) {
        delete get_str_ptr(data);
    }
}

Symbol::Symbol(const Symbol& other) {
    if (is_short_string(other)) {
        data = other.data;
    } else {
        data = get_str_data(new std::string(*get_str_ptr(other.data)));
    }
}

void swap(Symbol& first, Symbol& second) noexcept {
    using std::swap;
    swap(first.data, second.data);
}


Symbol::Symbol(Symbol&& other) : data {} {
    swap(*this, other);
}

Symbol& Symbol::operator=(Symbol other) {
    swap(*this, other);
    return *this;
}

bool operator==(const Symbol& a, const Symbol& b) {
    return temp_str(a) == temp_str(b);
}

Symbol operator+(const Symbol& a, const Symbol& b) {
    return Symbol {temp_str(a) + temp_str(b)};
}

std::ostream& operator<<(std::ostream& os, const Symbol& x) {
    os << temp_str(x);
    return os;
}

const Any t {Symbol {"t"}};
const Any True {t};

