//
// Created by anthony on 6/28/24.
//
#include <iostream>
#include <cassert>
#include <csstring.h>  // Assuming this is the file where your Any class is defined
#include "any.h"
#include "builtin_functions.h"
#include "op_overload.h"

int test_string_any() {
    // Test memory layout
    static_assert(sizeof(Any) == sizeof(String));
    // Test initialization

    Any s1 {String("Hello, World!")};
    assert(s1 == String("Hello, World!"));

    // Test copying
    Any s2 = s1;
    assert(s2 == s1);

    // Test indexing
    assert(s1[0] == String('H'));
    assert(s1[1] == String('e'));

    // Test immutability
    // s1[0] = 'h';  // This should cause a compile error
    Any s11 = String("World");
    assert(len(s11) == 5);

    // Test find
    assert(find(s1, "World") == 7);
    assert(find(s1, "world") == -1);
    assert(find(s1, "o", 5) == 8);
    assert(find(s1, "o", 5, 7) == -1);

    // Test subseq
    assert(subseq(s1, 0, 5) == "Hello");
    assert(subseq(s1, -1) == "!");
    assert(subseq(s1, -5, -1) == "orld");

    // Test concatenation
    Any s1_cat = s1 + String(" How are you?");
    assert(s1_cat == "Hello, World! How are you?");
    s1 = s1 + String(" What's up?");
    assert(s1 == "Hello, World! What's up?");
    // assert modifying in-place is not allowed

    /// ISSUE SOLVED!
    Any t1 = String("Hello");
    Any t2 = t1;
    t1 = toupper(t1);
    assert(t2 == "Hello");
    assert(t1 == "HELLO");

    // Test toupper
    Any s3 {"hello, world!"};
    assert(toupper(s3) == "HELLO, WORLD!");

    // Add more tests!
    std::cout << "All tests passed!\n";
    return 0;
}

