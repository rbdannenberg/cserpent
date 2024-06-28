// csstring.cpp - implementation of Cserpent string object

#include "any.h"
#include "gc.h"
#include "obj.h"
#include "csstring.h"
#include "csmem.h"

String::String(const char *s) {
    set_tag(tag_string);
    std::string &str = *new(slots) (std::string)(s);
}
