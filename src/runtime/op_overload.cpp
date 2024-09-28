#include "any.h"
#include "basic_obj.h"
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

Any operator+ (Any lhs, const String& rhs) {
    if (is_str(lhs)) {
        return { to_str(lhs) + rhs};
    }
}

Any operator+ (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs + to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs + to_real(rhs);
    }
    else if (is_str(rhs)) {
        return lhs + to_str(rhs);
    }
    else type_error(rhs);
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

std::ostream& operator<<(std::ostream& os, Any x) {
    return os << force_str(x);
    switch (get_type(x)) {
        case Any_type::INT:
            return os << to_int(x);
        case Any_type::REAL:
            return os << to_real(x);
        case Any_type::STR:
            return os << to_str(x);
        case Any_type::SYMBOL:
            return os << to_symbol(x);
        case Any_type::ARRAY:
            return os << to_array(x);
        case Any_type::DICT:
            return os << to_dict(x);
        case Any_type::OBJ:
            return os << "Not implemented yet";
        case Any_type::NIL:
            return os << "nil";
        default:
            return os << "unknown";
    }
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
        return lhs == to_str(rhs);
    } else if (is_symbol(rhs)) {
        return lhs == to_symbol(rhs);
    }
    return lhs.integer == rhs.integer;
}

bool operator==(Any lhs, String rhs) {
    if (is_str(lhs)) {
        return to_str(lhs) == rhs;
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
//        Basic_obj *basic_ptr= to_ptr(*this);
//        if (basic_ptr->get_tag() == tag_array) {
//            assert(false);  // do not use a[i] = x; instead use a.set(i, x);
////            std::vector<Any> *data = (std::vector<Any> *) basic_ptr->slots;
////            return data->at(i);
////            Array arr (static_cast<Array_heap*>(basic_ptr));
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
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr = to_ptr(*this);
        if (basic_ptr->get_tag() == tag_array) {
            (static_cast<Array*>(basic_ptr))->set(i, val);
            return;
        }
    }
    type_error(*this);
}

Any Any::operator[](int64_t i) const {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr= to_ptr(*this);
        if (basic_ptr->get_tag() == tag_array) {
            return (*(static_cast<Array*>(basic_ptr)))[i];
        }
    } else if (is_str(*this)) {
        return String {to_str(*this)[i]};
    } else {
        type_error(*this);
    }
}

