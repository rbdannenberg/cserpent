#include <any.h>
#include <any_utils.h>
#include <op_overload.h>
#include <data_structures/array.h>
#include <data_structures/dictionary.h>
#include <data_structures/symbol.h>

#include <map>

//Symbol& Tree = *(new Symbol {"Tree"})
namespace global {
    std::map<std::string, Symbol&> symbol_table;
}

class Tree : public Obj { // change to Obj when Symbols are implemented
public:
    Any left;
    Any right;

    //Tree(): Obj("Tree") {}
    Tree(Any left, Any right): left(left), right(right) {}

    int64_t test(int64_t x) {
        return x * 2;
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
        if (member_name == "test") {
            //dispatch_error(member_name, args, kwargs, 1, 0);
            return test(args[0]);
        }
    }
};


//TODO: code to add "Tree" to the global symbol table

//Any::access(std::string member) {
//    if (is_ptr(*this)) {
//        // check tag: is user-defined class
//        reinterpret_cast<Obj*>(to_ptr(*this))->name
//    }
//}

Any make_tree(Any depth) {
    if (depth == 0) {
        return *(new Tree {3, 4});
    }
    depth = depth - 1;
    Any new_tree = *(new Tree {5, 6});
    std::cout << new_tree.get("left") << std::endl;

    Tree *test_tree = new Tree {5, 6};
    Obj *test1 = test_tree;
    Basic_obj *test2 = test_tree;
    delete test_tree;

    Tree test_tree_s {5, 6};
    Obj *test1_s = &test_tree_s;
    Basic_obj *test2_s = &test_tree_s;
    return {};

//    Tree& new_tree = *(new Tree {});
//    Any new_tree_test = *(new Tree {});
//
//    new_tree.test(5);
//    new_tree_test.call("test", Array {5}, Dictionary {})
//    //static_cast<Tree&>(new_tree_test).test(5);
//
//    new_tree.access("left")
//
//
//    new_tree.left = make_tree(depth);
//    new_tree.right = make_tree(depth);
//    return new_tree;
}

int main() {
    //Symbol tree_symbol {"Tree", &Tree::call, &Tree::get};
    //global::symbol_table["Tree"] =
    make_tree(3);
    return 0;
}
