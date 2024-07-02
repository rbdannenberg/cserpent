//
// Created by anthony on 6/4/24.
//
#pragma once
#include <string>
#include <utility>
#include <obj.h>
#include <limits>

// CSerpent string, with tag tag_string. For now, this is just
// a C++ std:string "boxed" in a Basic_obj so it can be GC'd:
/// There should be no naked std::strings in the compiled code.
/// Only String objects (on heap) and string literals (const char *)
class String : public Basic_obj {
public:
    // string takes 24 bytes, so we need 3 slots. Basic_obj has 1, so
    // we need 2 more. This expression is essentially roundup(24 / 8) - 1:
    int64_t more_slots[((sizeof(std::string) + sizeof(int64_t) - 1) /
                        sizeof(int64_t)) - 1];
    String(const char *s);
};

/* MERGE CONFLICTS BEGIN HERE */
/* Some constructors and other methods might be needed, but I don't think
 * these implementations will work as is ...

    const std::string str;

    //TODO: do these constructors need to set slots?
    String(std::string s) : str(std::move(s)) {
        set_tag(tag_string);
    }

    String(const char *s) : str(s) {
        set_tag(tag_string);
    }
    // TODO: rule of 3 - define copy constructor when copy assignment is approved?

//    // Default copy assignment will allocate memory on the heap to store the string data
//    String operator=(const String& other) {
//        // let garbage collector handle the old string
//        std::cout << "copy assignment" << std::endl;
//        String& result = *(new String {other.str});
//        return result;
//    }

    // Or should this return Any?
//    char operator[](int64_t i) {
//        return str[i];
//    }
};
 */

#ifdef HIDE_SOME_CODE
/* Some string methods were added for a different implementation. Like the class
 * declaration, some of these functions might be needed, but I don't think these
 * implementations will work as is ...
 */

String * create_string(const char *x) {
    return new String {x};

}

char index(const String *s, int64_t i) {
    return s->str[i];
}

String *subseq(const String *s, int64_t start, int64_t end = std::numeric_limits<int64_t>::max()) {
    if (end == std::numeric_limits<int64_t>::max()) {
        end = s->str.size();
    }
    if (end < 0) {
        end = s->str.size() + end;
    }
    if (start < 0) {
        start = s->str.size() + start;
    }
    return new String {s->str.substr(start, end - start)}; // substr constructs a new std::string
}

int64_t find(const String *s, const String *pattern, int64_t start=0, int64_t end=std::numeric_limits<int64_t>::max()) {
    if (end == std::numeric_limits<int64_t>::max()) {
        end = s->str.size();
    }
    start = start < 0 ? s->str.size() + start : start;
    end = end < 0 ? s->str.size() + end : end;
    std::string sub = s->str.substr(start, end - start);
//    std::cout << "sub: " << sub << std::endl;
    int64_t find_result = sub.find(pattern->str);
    return find_result == -1 ? -1 : find_result + start;
}

String *toupper(const String *s) {
    std::string upper;
    for (char c : s->str) {
        upper.push_back(std::toupper(c));
    }
    return new String {upper};
}

/**
 * Handles comparison with string literals as well. (String{"hello"} == "hello")
 * Can't avoid the creation of a std::string to use == operator.
 * String is just a wrapper around std::string.
 * Once s2 goes out of scope, the std::string it wraps will be deallocated.
 */
bool equals(const String *s1, const String *s2) {
    return s1->str == s2->str;
}


String *add(const String *s1, const String *s2) {
    return new String {s1->str + s2->str};
}


//const char *x = "hello";
//Any y = x;
//
//String x = "hello";
//Any y = x;
/// Preserve this if we decide to switch back to reference semantics.
namespace reference_semantics {
    int64_t
    find(const String &s, const String &pattern, int64_t start = 0, int64_t end = std::numeric_limits<int64_t>::max()) {
        if (end == std::numeric_limits<int64_t>::max()) {
            end = s.str.size();
        }
        start = start < 0 ? s.str.size() + start : start;
        end = end < 0 ? s.str.size() + end : end;
        std::string sub = s.str.substr(start, end - start);
//    std::cout << "sub: " << sub << std::endl;
        int64_t find_result = sub.find(pattern.str);
        return find_result == -1 ? -1 : find_result + start;
    }

    String toupper(const String &s) {
        std::string upper;
        for (char c: s.str) {
            upper.push_back(std::toupper(c));
        }
        return String{upper};
    }

/**
 * Handles comparison with string literals as well. (String{"hello"} == "hello")
 * Can't avoid the creation of a std::string to use == operator.
 * String is just a wrapper around std::string.
 * Once s2 goes out of scope, the std::string it wraps will be deallocated.
 */
    bool operator==(const String &s1, const String &s2) {
        return s1.str == s2.str;
    }


    String operator+(const String &s1, const String &s2) {
        return String{s1.str + s2.str};
    }
}

#endif

/* MERGE CONFLICTS END HERE */
