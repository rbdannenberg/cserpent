//
// Created by anthony on 7/4/24.
//

///@file This should only be used for testing if we are using the vtable imeplementation of dynamic dispatch.
/// Thus, it is currently deprecated.

#include "obj.h"

class Derived: public Obj {

    Any call(const std::string& member_name, const Array& args, const Dictionary& kwargs) override {
        std::cout << "no member functions" << std::endl;
    };
    Any& get(const std::string& member_name) {
        std::cout << "no member variables" << std::endl;
    }
};

int main() {
    auto o_ptr = new Obj {};
    auto d_ptr = new Derived {};
    delete o_ptr;
    delete d_ptr;
    return 0;
}