//
// Created by anthony on 7/2/24.
//

#include <stdexcept>
#include <cassert>
#include <utility>
#include "csstring.h"
#include "any.h"


// distinguish between short and long strings by the 47th bit. Pointers will have 1, short
// strings will have 0, for the nul terminator.
// number of useful characters in a short string (i.e. does not include the nul terminator)
static constexpr int64_t max_short_size = 5;
static constexpr uint16_t string_tag = 0xFFFA;
static constexpr uint64_t long_tag = 0xFFFA800000000000; // for long strings only, 0xFFFA for regular tag, 8 to distinguish between nul terminator
static constexpr uint64_t empty_string = 0xFFFA000000000000;

// O(max_short_size)
static bool is_short_literal(const char * literal) {
    for (int64_t i = 0; i < max_short_size + 1; i++) {
        if (literal[i] == '\0') return true;
    }
    return false;
}

static bool is_short_string(const String& s) {
    return s.chars[5] == '\0';
}

static std::string * get_str_ptr(uint64_t s) {
    return reinterpret_cast<std::string*>(s & ~long_tag); // you might worry about the 47th bit, but that ensures we get a "canonical" pointer
}

static uint64_t get_str_data(std::string * s) {
    return reinterpret_cast<uint64_t>(s) | long_tag;
}

/// @brief temporary conversion to exploit the std::string library
/// static so we don't expose it outside of this file
/// I believe std::string has optimizations for short strings as well, so this shouldn't be too wasteful
static std::string temp_str(const String& s) {
    if (is_short_string(s)) {
        std::string result {s.chars}; // should stop upon nul terminator
        assert(result.size() == len(s));
        return result;
    } else {
        return *get_str_ptr(s.data);
    }
}

String::String() : data {empty_string} {}
String::String(char c) {
    tag = string_tag;
    chars[0] = c;
    chars[1] = '\0';
    chars[5] = '\0';
}

String::String(const char* literal) {
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
String::String(const std::string& s) : String {s.c_str()} {}

String::~String() {
    if (!is_short_string(*this)) {
        delete get_str_ptr(data);
    }
}

String::String(const String& other) {
    if (is_short_string(other)) {
        data = other.data;
    } else {
        auto new_str = new std::string {*get_str_ptr(other.data)};
        data = get_str_data(new_str);
//        data = get_str_data(new std::string(*get_str_ptr(other.data)));
    }
}

void swap(String& first, String& second) noexcept {
    using std::swap;
    swap(first.data, second.data);
}


String::String(String&& other) : data {} {
    swap(*this, other);
}

String& String::operator=(String other) {
    swap(*this, other);
    return *this;
}

char String::operator[](int64_t i) const {
    if (is_short_string(*this)) {
        return chars[i];
    } else {
        return (*get_str_ptr(data))[i];
    }
}

int64_t len(const String& s) {
    if (is_short_string(s)) {
        for (int64_t i = 0; i <= max_short_size; i++) { // len can be anything from 0 to 5
            if (s.chars[i] == '\0') return i;
        }
    } else {
        return get_str_ptr(s.data)->size();
    }
}

String subseq(const String& s, int64_t start, int64_t end) {
    int64_t s_len = len(s);
    if (end == std::numeric_limits<int64_t>::max()) {
        end = s_len;
    }
    if (end < 0) {
        end = s_len + end;
    }
    if (start < 0) {
        start = s_len + start;
    }
    // do bounds checking: 0 <= start <= end <= s_len
    if (start < 0 || start > end || end > s_len) {
        throw std::out_of_range("subseq: out of range");
    }
    if (is_short_string(s)) {
        String result {};
        for (int64_t i = start; i < end; i++) {
            assert(0 <= i - start && i - start < max_short_size);
            assert(0 <= i && i < max_short_size);
            result.chars[i - start] = s.chars[i];
        }
        return result;
    } else {
        return String {get_str_ptr(s.data)->substr(start, end - start)};
    }
}

int64_t find(const String& s, const String& pattern, int64_t start, int64_t end) {
    int64_t s_len = len(s);
    if (end == std::numeric_limits<int64_t>::max()) {
        end = s_len;
    }
    if (end < 0) {
        end = s_len + end;
    }
    if (start < 0) {
        start = s_len + start;
    }
    // do bounds checking: 0 <= start <= end <= s_len
    if (start < 0 || start > end || end > s_len) {
        throw std::out_of_range("subseq: out of range");
    }
    // value semantics let us exploit move semantics
    std::string sub = temp_str(subseq(s, start, end));
    int64_t find_result = sub.find(temp_str(pattern));
    return find_result == -1 ? -1 : find_result + start;
}

String toupper(const String& s) {
    if (is_short_string(s)) {
        String result {};
        for (int64_t i = 0; i < len(s); i++) {
            result.chars[i] = std::toupper(s.chars[i]);
        }
        return result;
    } else {
        std::string upper;
        for (char c : *get_str_ptr(s.data)) {
            upper.push_back(std::toupper(c));
        }
        return String {upper};
    }
}

String tolower(const String& s) {
    if (is_short_string(s)) {
        String result {};
        for (int64_t i = 0; i < len(s); i++) {
            result.chars[i] = std::tolower(s.chars[i]);
        }
        return result;
    } else {
        std::string upper;
        for (char c : *get_str_ptr(s.data)) {
            upper.push_back(std::tolower(c));
        }
        return String {upper};
    }
}

bool operator==(const String& a, const String& b) {
    return temp_str(a) == temp_str(b);
}

String operator+(const String& a, const String& b) {
    return String {temp_str(a) + temp_str(b)};
}

std::ostream& operator<<(std::ostream& os, const String& x) {
    os << temp_str(x);
    return os;
}

int64_t ord(const String& s) {
    if (is_short_string(s)) return s.chars[0];
    else return get_str_ptr(s.data)->at(0);
}

String chr(int64_t i) {
    return String {static_cast<char>(i)};
}

String strcat(const String& lhs, const String& rhs) {
    return String {temp_str(lhs) + temp_str(rhs)};
}

