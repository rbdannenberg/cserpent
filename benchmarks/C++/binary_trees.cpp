#include <any.h>
#include <op_overload.h>
#include <data_structures/array.h>
#include <data_structures/dictionary.h>
#include <builtin_functions/builtin_functions.h>


class Tree : public Obj {
public:
    Any left;
    Any right;

    Any& get(const std::string& member_name) override {
        if (member_name == "left") {
            return left;
        }
        else if (member_name == "right") {
            return right;
        }
    }

    Any call(const std::string& member_name, const Array& args, const Dictionary& kwargs) override {
        std::cout << "no member functions" << std::endl;
    }
};

Any make_tree(Any depth) {
    if (depth == 0) {
        return *(new Tree {});
    }
    depth = depth - 1;
    Any new_tree = *(new Tree {});

    new_tree.get("left") = make_tree(depth);
    new_tree.get("right") = make_tree(depth);
    return new_tree;
}

Any check_tree(Any tree) {
    if (tree.get("left") == Any {}) {
        return 1;
    }
    return check_tree(tree.get("left")) + check_tree(tree.get("right")) + 1;
}

void test_trees(Any arg) {
    Any min_depth = 4;
    Any max_depth = arg;
    Any stretch_depth = max_depth + 1;
    std::cout << "stretch tree of depth " << stretch_depth << " check: " << check_tree(make_tree(stretch_depth)) << std::endl;

    Any long_lived_tree = make_tree(max_depth);
    Any iterations = pow(2, max_depth);
    for (int64_t depth = min_depth; depth < stretch_depth; depth+=2) {
        Any check = 0;
        for (int64_t i = 0; i < iterations; i++) {
            check = check + check_tree(make_tree(depth));
        }
        std::cout << iterations << " trees of depth " << depth << " check: " << check << std::endl;
        iterations = idiv(iterations, 4);
    }
    std::cout << "long lived tree of depth " << to_int(max_depth) << " check: " << check_tree(long_lived_tree) << std::endl;
}
