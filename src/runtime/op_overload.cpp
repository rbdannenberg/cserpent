#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "any_utils.h"
#include "symbol.h"
#include "op_overload.h"
#include "array.h"
#include "csstring.h"
#include <iostream>

// ARITHMETIC OPERATORS

// Right now, we are ignoring the fact that strings and lists can be added to
Any operator+ (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) + rhs };
    }
    else if (is_real(lhs)) {
        return { to_real(lhs) + static_cast<double>(rhs) };
    }
    else type_error(lhs);
}

Any operator+ (Any lhs, int rhs) {
    return lhs + static_cast<int64_t>(rhs);
}

Any operator+ (Any lhs, double rhs) {
    return { force_real(lhs) + rhs };
}

/* Any operator+ (Any lhs, const String *rhs) {
    assert(false);  // needs work
    if (is_string(lhs)) {
        assert(false);  // needs work
        // return { to_string(lhs) + rhs};
    } else if (is_short(lhs)) {
        return nil;
    }
}
 */

Any operator+ (Any lhs, StringPtr rhs) {
    if (is_string(lhs)) {
        String* lhs_str = to_string(lhs);
        std::string result = *lhs_str->get_string() + *rhs.ptr->get_string();
        return Any(new String(result));
    } else type_error(lhs);
}

Any operator+ (StringPtr lhs, Any rhs) {
    if (is_string(rhs)) {
        String* rhs_str = to_string(rhs);
        std::string result = *lhs.ptr->get_string() + *rhs_str->get_string();
        return Any(new String(result));
    } else type_error(rhs);
}

Any operator+ (Any lhs, const char *rhs) {
    if (is_string(lhs)) {
        const char *lhs_c_str = get_c_str(&lhs);
        std::string result(lhs_c_str);
        result += rhs;
        return Any(new String(result));
    } else {
        assert(false);
    }
}

Any operator+ (Any lhs, Any rhs) {
    if (is_string(rhs)) {
        return lhs + get_c_str(&rhs);
    } else if (is_int(rhs)) {
        return lhs + to_int(rhs);
    } else if (is_real(rhs)) {
        return lhs + to_real(rhs);
    } else type_error(rhs);
}

// * operators
Any operator* (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) * rhs };
    }
    else if (is_real(lhs)) {
        return { to_real(lhs) * static_cast<double>(rhs) };
    }
    else type_error(lhs);
}

Any operator* (Any lhs, int rhs) {
    return lhs * static_cast<int64_t>(rhs);
}

Any operator* (Any lhs, double rhs) {
    return { force_real(lhs) * rhs };
}

Any operator* (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs * to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs * to_real(rhs);
    }
    else type_error(rhs);
}

// Symmetrical * operators
Any operator* (int64_t lhs, Any rhs) {
    return rhs * lhs;
}

Any operator* (int lhs, Any rhs) {
    return rhs * lhs;
}

Any operator* (double lhs, Any rhs) {
    return rhs * lhs;
}

// - operators
Any operator- (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) - rhs };
    } else if (is_real(lhs)) {
        return { to_real(lhs) - static_cast<double>(rhs) };
    } else type_error(lhs);
}

Any operator- (Any lhs, int rhs) {
    return lhs - static_cast<int64_t>(rhs);
}

Any operator- (Any lhs, double rhs) {
    return { force_real(lhs) - rhs };
}

Any operator- (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs - to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs - to_real(rhs);
    }
    else type_error(rhs);
}

Any operator- (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return { lhs - to_int(rhs) };
    } else if (is_real(rhs)) {
        return { static_cast<double>(lhs) - to_real(rhs) };
    } else type_error(rhs);
}

Any operator- (int lhs, Any rhs) {
    return static_cast<int64_t>(lhs) - rhs;
}

Any operator- (double lhs, Any rhs) {
    return lhs - force_real(rhs);
}

// / operators
double operator/ (double lhs, Any rhs) {
    return lhs / force_real(rhs);
}

double operator/ (int64_t lhs, Any rhs) {
    return static_cast<double>(lhs) / force_real(rhs);
}

double operator/ (Any lhs, Any rhs) {
    return force_real(lhs) / force_real(rhs);
}

double operator/ (Any lhs, double rhs) {
    return force_real(lhs) / rhs;
}

double operator/ (Any lhs, int64_t rhs) {
    return force_real(lhs) / static_cast<double>(rhs);
}

// COMPARISON OPERATORS
bool operator< (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs < to_int(rhs);
    }
    else if (is_real(rhs)) {
        return static_cast<double>(lhs) < to_real(rhs);
    }
    else type_error(rhs);
}

bool operator< (double lhs, Any rhs) {
    return lhs < force_real(rhs);
}

bool operator< (Any lhs, Any rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) < rhs;
    }
    else if (is_real(lhs)) {
        return to_real(lhs) < rhs;
    }
    else type_error(lhs);
}

bool operator< (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) < rhs;
    }
    else if (is_real(lhs)) {
        return to_real(lhs) < static_cast<double>(rhs);
    }
    else type_error(lhs);
}

bool operator< (Any lhs, double rhs) {
    return force_real(lhs) < rhs;
}

bool operator> (Any lhs, Any rhs) {
    return !(lhs < rhs || lhs == rhs);
}

bool operator>= (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) >= rhs;
    } else type_error(lhs);
}

bool operator>= (Any lhs, Any rhs) {
    return ! (lhs < rhs);
}

bool operator<= (Any lhs, Any rhs) {
    return !(lhs > rhs);
}

// BITWISE OPERATORS

// &= operators
int64_t operator&= (int64_t& lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs &= to_int(rhs);
    }
    else type_error(rhs);
}

int64_t operator&= (Any& lhs, int64_t rhs) {
    if (is_int(lhs)) {
        int64_t _tmp = to_int(lhs) & rhs;
        lhs = _tmp;
        return _tmp;
    }
    else {
        type_error(lhs);
    }
}

int64_t operator&= (Any& lhs, Any rhs) {
    if (is_int(rhs) && is_int(lhs)) {
        int64_t _tmp = to_int(lhs) & to_int(rhs);
        lhs = _tmp;
        return _tmp;
    }
    else {
        type_error(lhs);
    }
}

// & operators

int64_t operator& (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs & to_int(rhs);
    } else type_error(rhs);
}

int64_t operator& (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) & rhs;
    } else type_error(lhs);
}

int64_t operator& (Any lhs, Any rhs) {
    if (is_int(lhs) && is_int(rhs)) {
        return to_int(lhs) & to_int(rhs);
    } else type_error(lhs);
}

int64_t operator<< (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs << to_int(rhs);
    }
    else type_error(rhs);
}

int64_t operator<< (int lhs, Any rhs) {
    if (is_int(rhs)) {
        return static_cast<int64_t>(lhs) << to_int(rhs);
    } else type_error(rhs);
}

int64_t operator<< (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) << rhs;
    } else type_error(lhs);
}

int64_t operator<< (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) << static_cast<int64_t>(rhs);
    } else type_error(lhs);
}

int64_t operator<< (Any lhs, Any rhs) {
    if (is_int(lhs) && is_int(rhs)) {
        return to_int(lhs) << to_int(rhs);
    } else type_error(lhs);
}

int64_t operator| (Any lhs, Any rhs) {
    if (is_int(lhs) && is_int(rhs)) {
        return to_int(lhs) | to_int(rhs);
    } else type_error(lhs);
}

int64_t operator| (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs | to_int(rhs);
    } else type_error(rhs);
}

int64_t operator| (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) | rhs;
    } else type_error(lhs);
}

int64_t operator^ (Any lhs, Any rhs) {
    if (is_int(lhs) && is_int(rhs)) {
        return to_int(lhs) ^ to_int(rhs);
    } else type_error(lhs);
}

int64_t operator^ (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs ^ to_int(rhs);
    } else type_error(rhs);
}

int64_t operator^ (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) ^ rhs;
    } else type_error(lhs);
}

int64_t operator>> (int64_t lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs >> to_int(rhs);
    } else type_error(rhs);
}

int64_t operator>> (Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) >> rhs;
    } else type_error(lhs);
}

std::ostream& operator<<(std::ostream& os, Any a) {
    //Any s(force_str(x));
    //return os << get_c_str(&s);
    if (is_string(a)) {
        os << get_c_str(&a);
    } else if (is_int(a)) {
        os << to_int(a);
    } else if (is_real(a)) {
        os << to_real(a);
    } else {
        os << "<unknown Any>";
    }
    return os;
}

bool operator==(Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) == rhs;
    } else type_error(lhs, __func__);
}

bool operator==(Any lhs, int rhs) {
    return lhs == static_cast<int64_t>(rhs);
}

bool operator==(Any lhs, Any rhs) {
    if (is_str(rhs)) {
        assert(false);  // needs work
        // return lhs == to_string(rhs);
    } else if (is_symbol(rhs)) {
        assert(false);  // needs work
        // return lhs == to_symbol(rhs);
    }
    return lhs.integer == rhs.integer;
}

bool operator==(Any lhs, String rhs) {
    if (is_str(lhs)) {
        return to_string(lhs) == rhs;
    }
    else type_error(lhs, __func__);
}

bool operator==(Any lhs, StringPtr rhs) {
    if (is_string(lhs)) {
        String* lhs_str = to_string(lhs);
        return *lhs_str->get_string() == *(rhs.ptr->get_string());
    }
    else type_error(lhs, __func__);
}

bool operator==(Any lhs, Symbol rhs) {
    if (is_symbol(lhs)) {
        return to_symbol(lhs) == rhs;
    }
    else type_error(lhs, __func__);
}

bool operator!=(Any lhs, Any rhs) {
    return !(lhs == rhs);
}

//Any& Any::operator[](int64_t i) {
//    if (is_ptr(*this)) {
//        Heap_obj *heap_obj= to_heap_obj(*this);
//        if (heap_obj->get_tag() == tag_array) {
//            assert(false);  // do not use a[i] = x; instead use a.set(i, x);
////            std::vector<Any> *data = (std::vector<Any> *) heap_obj->slots;
////            return data->at(i);
////            Array arr (static_cast<Array_heap*>(heap_obj));
////            return arr[i];
//        }
//    }
////    else if (is_shortstr(*this)) {
////        std::string str = to_shortstr(*this);
////        if (i < str.length()) {
////            // doesn't this defeat the purpose
////            // return a char?
////            return Any {str[i]};
////        }
////    }
//// Note: This is because strings should be immutable
//    else {
//        type_error(*this);
//    }
//}
void Any::set(int64_t i, Any val) {
    if (is_heap_obj(*this)) {
        Heap_obj *heap_obj = to_heap_obj(*this);
        if (heap_obj->get_tag() == tag_array) {
            (static_cast<Array*>(heap_obj))->set(i, val);
            return;
        }
    }
    type_error(*this);
}

Any Any::operator[](int64_t i) const {
    if (is_heap_obj(*this)) {
        Heap_obj *heap_obj= to_heap_obj(*this);
        if (heap_obj->get_tag() == tag_array) {
            return (*(static_cast<Array*>(heap_obj)))[i];
        }
    } else if (is_string(*this)) {
        // TODO: this is not correct for Unicode 
        int64_t len;
        const char *s = get_c_str(this, &len);
        if (i < 0 || i >= len) {
            throw std::out_of_range("String index is out of range");
        }
        return Any(s[i]);
    } else {
        type_error(*this);
    }
}

