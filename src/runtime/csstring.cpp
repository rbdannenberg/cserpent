//
// Created by anthony on 7/2/24.
//

#include <stdexcept>
#include <cassert>
#include <utility>
#include <cctype>
#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "op_overload.h"
#include "csstring.h"

int64_t len(String *x) { return x->len(); }

/*
static std::string * get_str_ptr(uint64_t s) {
     // you might worry about the 47th bit, but that ensures we get a
     // "canonical" pointer
    return reinterpret_cast<std::string*>(s & ~long_mask);
}


static uint64_t str_pointer_to_data(std::string * s) {
    return reinterpret_cast<uint64_t>(s) | long_str_tag;
}


/// @brief temporary conversion to exploit the std::string library
/// static so we don't expose it outside of this file
/// I believe std::string has optimizations for short strings as well, so this shouldn't be too wasteful
std::string temp_str(const String& s) {
    if (is_short_string(s)) {
        // should stop upon nul terminator
        std::string result {s.chars + STR_BASE};
        assert(result.size() == len(s));
        return result;
    } else {
        return *get_str_ptr(s.data);
    }
}
*/

String::String(const char *s) {
    set_tag(tag_string);
    slots[0].integer = (int64_t) (new std::string(s ? s : ""));
}


String::String(const std::string &s) {
    set_tag(tag_string);
    slots[0].integer = (int64_t) (new std::string(s));
}

/*
uint64_t literal_string_to_data(const char *literal) {
    uint64_t data;
    if (is_short_literal(literal)) {
        return STR_TAG;
        char *chars = reinterpret_cast<char *>(data);
        for (int64_t i = 0; i < max_short_size; i++) {
            chars[STR_BASE + i] = literal[i];
            chars[5] = '\0';
        }
    } else {
        return str_pointer_to_data(new std::string(literal));
    }
}

String::String(const char* literal) :
    data { literal_string_to_data(literal) }  {}


String::String(const std::string& s) : String {s.c_str()} {}


String::~String() {
    if (!is_short_string(*this)) {
        delete get_str_ptr(data);
    }
}


uint64_t copy_string_to_data(const String& other) {
    if (is_short_string(other)) {
        return other.data;
    } else {
        auto new_str = new std::string {*get_str_ptr(other.data)};
        return str_pointer_to_data(new_str);
    }
}


String::String(const String& other) : 
    data { copy_string_to_data(other) } { }


void swap(String& first, String& second) noexcept {
    using std::swap;
    swap(first.data, second.data);
}
*/

/* WHAT IS THIS?
String::String(String&& other) : data {} {
    swap(*this, other);
}
*/

/* Overloading assignment to do swap?
String& String::operator=(String other) {
    swap(*this, other);
    return *this;
}
*/

/*
char String::operator[](int64_t i) const {
    if (is_short_string(*this)) {
        return chars[STR_BASE + i];
    } else {
        return (*get_str_ptr(data))[i];
    }
}

int64_t len(const String& s) {
    // len can be anything from 0 to 4
    if (is_short_string(s)) {
        for (int64_t i = 0; i <= max_short_size; i++) {
            if (s.chars[i] == '\0') {
                return i;
            }
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
            // check destination index in bounds:
            assert(0 <= i - start && i - start < max_short_size);
            // check source index in bounds:
            assert(0 <= i && i < max_short_size);
            result.chars[STR_BASE + i - start] = s.chars[STR_BASE + i];
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
            result.chars[STR_BASE + i] = std::toupper(s.chars[STR_BASE + i]);
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
            result.chars[STR_BASE + i] = std::tolower(s.chars[STR_BASE + i]);
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


*/
String operator+(const String& lhs, const String& rhs) {
    std::string* left = lhs.get_string();
    std::string* right = rhs.get_string();
    return String(*left + *right);
}

String* operator+(const String* lhs, const String& rhs) {
    if (lhs) {
        std::string* left = lhs->get_string();
        std::string* right = rhs.get_string();
        return new String(*left + *right);
    }
    return NULL;
}

std::ostream& operator<<(std::ostream& os, const String& x) {
    os << x.get_c_str();
    return os;
}

std::ostream& operator<<(std::ostream& os, String* x) {
    if (x) os << *(x->get_string());
    else os << "<null String>";
    return os;
}

std::ostream& operator<<(std::ostream& os, StringPtr x) {
    if (x.ptr) {
        os << *(x.ptr->get_string());
    } else {
        os << "<null StringPtr>";
    }
    return os;
}

bool operator==(StringPtr lhs, StringPtr rhs) {
    if (lhs.ptr && rhs.ptr) {
        return *(lhs.ptr->get_string()) == *(rhs.ptr->get_string());
    }
    return lhs.ptr == rhs.ptr;  // both null or one null
}

StringPtr operator+(StringPtr lhs, StringPtr rhs) {
    if (lhs.ptr && rhs.ptr) {
        std::string* left = lhs.ptr->get_string();
        std::string* right = rhs.ptr->get_string();
        return StringPtr(new String(*left + *right));
    }
    return StringPtr(nullptr);
}

StringPtr subseq(StringPtr s, int64_t start, int64_t end) {
    if (!s.ptr) {
        throw std::invalid_argument("subseq: null StringPtr");
    }
    int64_t s_len = s.ptr->len();
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
    std::string* str = s.ptr->get_string();
    return StringPtr(new String(str->substr(start, end - start)));
}

int64_t find(StringPtr s, StringPtr pattern, int64_t start, int64_t end) {
    if (!s.ptr) {
        throw std::invalid_argument("find: null StringPtr");
    }
    if (!pattern.ptr) {
        throw std::invalid_argument("find: null pattern StringPtr");
    }
    int64_t s_len = s.ptr->len();
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
    std::string sub = s.ptr->get_string()->substr(start, end - start);
    int64_t find_result = sub.find(*(pattern.ptr->get_string()));
    return find_result == -1 ? -1 : find_result + start;
}

StringPtr toupper(StringPtr s) {
    if (!s.ptr) {
        throw std::invalid_argument("toupper: null StringPtr");
    }
    std::string* str = s.ptr->get_string();
    std::string upper;
    for (char c : *str) {
        upper.push_back(std::toupper(c));
    }
    return StringPtr(new String(upper));
}

StringPtr tolower(StringPtr s) {
    if (!s.ptr) {
        throw std::invalid_argument("tolower: null StringPtr");
    }
    std::string* str = s.ptr->get_string();
    std::string lower;
    for (char c : *str) {
        lower.push_back(std::tolower(c));
    }
    return StringPtr(new String(lower));
}

/*

int64_t ord(const String& s) {
    if (is_short_string(s)) return s.chars[STR_BASE + 0];
    else return get_str_ptr(s.data)->at(0);
}


String chr(int64_t i) {
    return String {static_cast<char>(i)};
}


String strcat(const String& lhs, const String& rhs) {
    return String {temp_str(lhs) + temp_str(rhs)};
}

*/
