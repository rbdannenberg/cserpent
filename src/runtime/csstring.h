//
// Created by anthony on 7/2/24.
//

// A long string is represented by a heap object Big_string, which
// holds the address of a C++ std::string. We need this level of
// indirection (variable points to heap object with a C++ string
// object which wraps a pointer to actual string data) instead of
// simply using C++ string because string uses smart-pointer
// techniques, so it's not simply a one-word pointer. Also, it must
// participate in our garbage collection world, where essentially
// everything is in the heap. Looked at another way, a CSerpent string
// is like a Serpent "extern" which we represent as a pointer wrapped
// in a heap object.
//
// Heap objects can be anything, but we'd like to be able to declare a
// value as "string" in CSerpent rather than "var" which gives us a
// dynamically-typed Any. Since we also support short strings nan-boxed
// into the same bits that might be a Big_string pointer, we just make
// String be identical to Any. There is a potential small savings from
// knowing that a String can only be a short string or a Big_string or
// nil, and our compiler keeps track of 'string' vs 'any', so we'll see
// how that plays out.

#pragma once
#include <string>
#include <cstdint>
#include <limits>
#include <iostream>

typedef Any String;

class Big_string : public Heap_obj {
private:  // add some room so we can store a std::string here:
    char padding[sizeof(std::string) - sizeof(int64_t)];
public:
    Big_string(const std::string &s);
    Big_string(const char *s = NULL);
    std::string *get_string() const {
        return (std::string *) slots; }
    const char *get_c_str() const {
        return ((std::string *) slots)->c_str(); }
    int64_t len() const {
        return ((std::string *) slots)->size(); }
};

// moved here from gc.h to remove forward reference to String:
void make_string_gray(String s);

/*
std::ostream& operator<<(std::ostream& os, const String &x);

std::ostream& operator<<(std::ostream& os, Big_string &x);
*/
/*String operator+(const String &lhs, const String &rhs);
*/

String subseq(const char *s, int64_t start,
              int64_t end = std::numeric_limits<int64_t>::max());

/*String toupper(String &s);
String tolower(String &s);
*/
