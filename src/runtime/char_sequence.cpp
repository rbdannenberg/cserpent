//
// Created by anthony on 7/7/24.
//

#include "char_sequence.h"
#include <stdexcept>
#include <cassert>
#include <utility>
// O(max_short_size)
int16_t CharSequence::get_short_tag() {
    throw std::runtime_error("Should not call from base");
    return {};
}

uint64_t CharSequence::get_long_tag() {
    throw std::runtime_error("Should not call from base");
    return {};
}

uint64_t CharSequence::get_empty_seq() {
    throw std::runtime_error("Should not call from base");
    return {};
}

bool CharSequence::is_short_literal(const char * literal) {
    for (int64_t i = 0; i < max_short_size + 1; i++) {
        if (literal[i] == '\0') return true;
    }
    return false;
}

bool CharSequence::is_short_seq(const CharSequence& s) {
    return s.chars[5] == '\0';
}

std::string * CharSequence::get_seq_ptr(uint64_t s) {
    return reinterpret_cast<std::string*>(s & ~(get_long_tag())); // you might worry about the 47th bit, but that ensures we get a "canonical" pointer
}

uint64_t CharSequence::get_seq_data(std::string * s) {
    return reinterpret_cast<uint64_t>(s) | (get_long_tag());
}

/// @brief temporary conversion to exploit the std::string library
/// static so we don't expose it outside of this file
/// I believe std::string has optimizations for short strings as well, so this shouldn't be too wasteful
std::string CharSequence::temp_str(const CharSequence& s) {
    if (is_short_seq(s)) {
        std::string result {s.chars}; // should stop upon nul terminator
        assert(result.size() == len(s));
        return result;
    } else {
        return *get_seq_ptr(s.data);
    }
}

CharSequence::CharSequence() : data {get_empty_seq()} {}
CharSequence::CharSequence(const char* literal) {
    if (is_short_literal(literal)) {
        for (int64_t i = 0; i < max_short_size; i++) {
            tag = get_short_tag();
            chars[i] = literal[i];
            chars[5] = '\0';
        }
    } else {
        data = get_seq_data(new std::string(literal));
    }
}
CharSequence::CharSequence(const std::string& s) : CharSequence {s.c_str()} {}

int64_t len(const CharSequence& s) {
    if (CharSequence::is_short_seq(s)) {
        for (int64_t i = 0; i <= CharSequence::max_short_size; i++) { // len can be anything from 0 to 5
            if (s.chars[i] == '\0') return i;
        }
    } else {
        return CharSequence::get_seq_ptr(s.data)->size();
    }
}
