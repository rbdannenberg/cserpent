//
// Created by anthony on 7/2/24.
//

#pragma once
#include <string>
#include <cstdint>
#include <limits>

/**
 * @brief Strings implemented with value semantics. The reason we can
 * (and should) do this is because strings are immutable, unlike other
 * data structures on the heap.  In short, define String to be a class
 * that fits within 64 bits, specifically as a union between a
 * fixed-length char array (for short strings) and a pointer to a
 * heap-allocated std::string (for long strings).  String will manage
 * the memory of its member std::string exclusively, so we remove it
 * from the purview of the garbage collector.  Reference counting is
 * not needed because every allocated std::string should only have 1
 * reference.
 */

// In a little-endian machine, short strings start at byte 0,
// and the tag bits are bytes 6 & 7 in an 8-byte array.
// In a big-endian machine, short strings start at byte 2,
// and the tag bits are bytes 0 & 1
//
// Strings have an extra type bit at bit 47 to distinguish short
// and long strings, so only 4 bytes + EOS are available for
// short strings.

#ifdef LITTLE_ENDIAN_ORDER
#define STR_BASE 0
#else
#define STR_BASE 2
#endif

union String {
    char chars[8];  // short string data starting at 0 or 2
    uint64_t data;  // long string or TAG

    //TODO: upon construction of a symbol, we should check if it's
    //already in the symbol table
    String(); // default constructor: the empty string

    // this should only be called internally, hence explicit
    explicit String(const std::string& s);
    
    // from string literals, consider making explicit and wrap every
    // literal in String():
    String(const char * literal);
    
    String(char c);

    // rule of 3 goes here
    friend void swap(String& first, String& second) noexcept;
    ~String();
    String(const String& other);
    /* String(String&& other); ?? */
    String& operator=(String other); // covers both copy and move assignment

    char operator[](int64_t i) const;

    friend int64_t ord(const String& s);
    friend String strcat(const String& lhs, const String& rhs);
};

int64_t len(const String& s);
String subseq(const String& s, int64_t start, int64_t end = std::numeric_limits<int64_t>::max());
int64_t find(const String& s, const String& pattern, int64_t start=0, int64_t end=std::numeric_limits<int64_t>::max());
String toupper(const String& s);
String tolower(const String& s);
bool operator==(const String& a, const String& b);
String operator+(const String& a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& x);

String chr(int64_t i);

// Additional functions used by Symbol implementation:
std::string temp_str(const String& s);
uint64_t literal_string_to_data(const char *literal);
uint64_t copy_string_to_data(const String& other);
