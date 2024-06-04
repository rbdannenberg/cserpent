//
// Created by anthony on 6/4/24.
//

#ifndef CSERPENT_SYMBOL_H
#define CSERPENT_SYMBOL_H



class Symbol : public Basic_obj {
public:
    // note: I removed the pointer here
    std::string name;
    Any value;
    void *fn_ptr;
    // add type information here?

    //Symbol(std::string name, Any value, void *ptr) :
    //  name {std::move(name)}, value {value}, fn_ptr {ptr} {}
};

#endif //CSERPENT_SYMBOL_H
