//
// Created by anthony on 6/4/24.
//
#pragma once
#include <string>
#include "obj.h"

// CSerpent string, with tag tag_string. For now, this is just
// a C++ std:string "boxed" in a Basic_obj so it can be GC'd:
//
class String : public Basic_obj {
public:
    std::string str;
};

const char *x = "hello";
Any y = x;

String x = "hello";
Any y = x;
