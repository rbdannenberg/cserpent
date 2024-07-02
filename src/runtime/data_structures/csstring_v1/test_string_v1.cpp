//
// Created by anthony on 6/28/24.
//
#include <iostream>
#include <cassert>
#include "data_structures/csstring_v1/csstring.h"  // Assuming this is the file where your String class is defined

void test_string() {
    // Test initialization
    String* s1 = new String {"Hello, World!"};
    assert(equals(s1, create_string("Hello, World!")));

    // Test copying
    String* s2 = s1;
    assert(s2 == s1);

    // Test indexing
    assert(index(s1, 0) == 'H');
    assert(index(s1, 1) == 'e');

    // Test immutability
    // s1[0] = 'h';  // This should cause a compile error


    // Test find
    assert(find(s1, create_string("World")) == 7);
    assert(find(s1, create_string("world")) == -1);
    assert(find(s1, create_string("o"), 5) == 8);
    assert(find(s1, create_string("o"), 5, 7) == -1);

    // Test subseq
    assert(equals(subseq(s1, 0, 5), create_string("Hello")));
    assert(equals(subseq(s1, -1), create_string("!")));
    assert(equals(subseq(s1, -5, -1), create_string("orld")));

    // Test concatenation
    String *s1_cat = add(s1, create_string(" How are you?"));
    assert(equals(s1_cat, create_string("Hello, World! How are you?")));
    s1 = add(s1, create_string(" What's up?"));
    equals(s1, create_string("Hello, World! What's up?"));
    // assert modifying in-place is not allowed
//    assert(s1->str == "Hello, World!");


    /// ISSUE:
    String *t1 = create_string("Hello");
    String *t2 = t1;
    t1 = create_string("World");
    assert(equals(t2, create_string("Hello"))); // This will fail
    assert(equals(t1, create_string("World")));

//    String& t1 = *(new String {"Hello"});
//    String& t2 = t1;
//    t1 = toupper(t1); // copy assignment. We want t1 to be set to a completely new String object on the heap.
//    // solution -
//    assert(t2.str == "Hello"); // This will fail
//    // Solution is either don't compile t1 = t1 ... or to modify the Any copy constructor/assignment to copy the string data


    // Test toupper
    String *s3 = create_string("hello, world!");
    assert(equals(toupper(s3), create_string("HELLO, WORLD!")));
    assert(equals(s3, create_string("hello, world!"))); // s3 should not be modified
}
