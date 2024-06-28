//
// Created by anthony on 6/28/24.
//
#include <iostream>
#include <cassert>
#include <data_structures/csstring.h>  // Assuming this is the file where your String class is defined

void test_string() {
    // Test initialization
    String& s1 = *(new String {"Hello, World!"});
    assert(s1.str == "Hello, World!");

    // Test copying
    String s2 = s1;
    assert(s2.str == s1.str);

    // Test indexing
    assert(s1[0] == 'H');
    assert(s1[1] == 'e');

    // Test immutability
    // s1[0] = 'h';  // This should cause a compile error


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
    assert(s1_cat.str == "Hello, World! How are you?");
    String *s1_ptr = &s1;
    std::cout << "here" << std::endl;
    s1 = s1 + " What's up?";
    std::cout << "here" << std::endl;
    std::cout << s1.str << std::endl;
    assert(s1 == "Hello, World! What's up?");
    // assert modifying in-place is not allowed
//    assert(s1_ptr->str == "Hello, World!");


    /// ISSUE:
    String& t1 = *(new String {"Hello"});
    String& t2 = t1;
    t1 = toupper(t1); // copy assignment. We want t1 to be set to a completely new String object on the heap.
    // solution -
    assert(t2.str == "Hello"); // This will fail
    // Solution is either don't compile t1 = t1 ... or to modify the Any copy constructor/assignment to copy the string data


    // Test toupper
    String s3 {"hello, world!"};
    assert(toupper(s3).str == "HELLO, WORLD!");
}
