//
// Created by anthony on 7/2/24.
//

#pragma once
#include <string>
#include <cstdint>
#include <limits>

/**
 * @brief Strings implemented with value semantics. The reason we can (and should) do this is because
 * strings are immutable, unlike other data structures on the heap.
 * In short, define String to be a class that fits within 64 bits, specifically as a union between a
 * fixed-length char array (for short strings) and a pointer to a heap-allocated std::string (for long strings).
 * String will manage the memory of its member std::string exclusively, so we remove it from the
 * purview of the garbage collector.
 * Reference counting is not needed because every allocated std::string should only have 1 reference.
 */


union String {
    struct {
        // short string
        // "wrong" way round because I hate little endian
        // tag must be last so it lines up with the most significant bytes of data/the pointer
        // nul terminator (chars[5]) is bits 44-47, tag is bits 48-63
        char chars[6];
        uint16_t tag;
    };
    uint64_t data; // long string

    String(); // default constructor: the empty string
    String(const char * literal); // from string literals, consider making explicit and wrap every literal in String()
    explicit String(const std::string& s); // this should only be called internally, hence explicit

    // rule of 3 goes here
    friend void swap(String& first, String& second) noexcept;
    ~String();
    String(const String& other);
    String(String&& other);
    String& operator=(String other); // covers both copy and move assignment

    char operator[](int64_t i) const;
};

int64_t len(const String& s);
String subseq(const String& s, int64_t start, int64_t end = std::numeric_limits<int64_t>::max());
int64_t find(const String& s, const String& pattern, int64_t start=0, int64_t end=std::numeric_limits<int64_t>::max());
String toupper(const String& s);
bool operator==(const String& a, const String& b);
String operator+(const String& a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& x);

