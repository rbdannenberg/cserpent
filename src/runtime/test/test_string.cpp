//
// Created by anthony on 6/28/24.
//
#include <iostream>
#include <cassert>
#include <data_structures/csstring.h>  // Assuming this is the file where your String class is defined

int main() {
    // Test initialization

    String s1 {"Hello, World!"};
    assert(s1 == "Hello, World!");

    // Test copying
    String s2 = s1;
    assert(s2 == s1);

    // Test indexing
    assert(s1[0] == 'H');
    assert(s1[1] == 'e');

    // Test immutability
    // s1[0] = 'h';  // This should cause a compile error
    String s11 = "World";
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
    String s1_cat = s1 + " How are you?";
    assert(s1_cat == "Hello, World! How are you?");
    String *s1_ptr = &s1;
    s1 = s1 + " What's up?";
    assert(s1 == "Hello, World! What's up?");
    // assert modifying in-place is not allowed

    /// ISSUE SOLVED!
    String t1 = "Hello";
    String t2 = t1;
    t1 = toupper(t1);
    assert(t2 == "Hello");
    assert(t1 == "HELLO");

    // Test toupper
    String s3 {"hello, world!"};
    assert(toupper(s3) == "HELLO, WORLD!");

    // Add more tests!
    std::cout << "All tests passed!\n";
    return 0;
}

