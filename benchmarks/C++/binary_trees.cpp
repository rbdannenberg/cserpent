#include <any.h>

//Symbol& Tree = *(new Symbol {"Tree"})

class Tree : public Basic_obj { // change to Obj when Symbols are implemented
public:
    Any left;
    Any right;
};

//TODO: code to add "Tree" to the global symbol table

Any::access(std::string member) {
    if (is_ptr(*this)) {
        // check tag: is user-defined class
        reinterpret_cast<Obj*>(to_ptr(*this))->name
    }
}

Any make_tree(Any depth) {
    if (depth == 0) {
        return *(new Tree {});
    }
    depth = depth - 1;
    Tree& new_tree = *(new Tree {});
    Any new_tree_test = *(new Tree {});
    new_tree.access("left")


    new_tree.left = make_tree(depth);
    new_tree.right = make_tree(depth);
    return new_tree;
}

int main() {
    make_tree(3);
    return 0;
}
