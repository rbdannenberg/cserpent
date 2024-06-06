#include <any.h>
#include <op_overload.h>
#include <data_structures/array.h>
#include <data_structures/dictionary.h>
#include <obj.h>
#include <iostream>


class Tree : public Obj {
public:
    Any left;
    Any right;

    Tree(Any left, Any right): left(left), right(right) {}

    void print() {
        std::cout << "Tree(" << left << ", " << right << ")" << std::endl;
    }

    Any get(const std::string& member_name) override {
        if (member_name == "left") {
            return left;
        }
        else if (member_name == "right") {
            return right;
        }
    }

    Any call(const std::string& member_name, const Array& args, const Dictionary& kwargs) override {
        if (member_name == "print") {
            check_dispatch(member_name, args, kwargs, 0, 0);
            print();
        }
    }
};

Any make_tree(Any depth) {
    Any new_tree = *(new Tree {5, 6});
    std::cout << new_tree.get("left") << std::endl;
    new_tree.call("print", Array {}, Dictionary {});
    return new_tree;
}

void test_trees() {
    make_tree(3);
}
