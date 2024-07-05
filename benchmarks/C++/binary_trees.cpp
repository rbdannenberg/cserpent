#include <any.h>
#include <op_overload.h>
#include <data_structures/array.h>
#include <data_structures/dictionary.h>
#include <builtin_functions/builtin_functions.h>
#include "benchmarking_utils.h"
#include <sstream>


class Tree : public Obj {
public:
    Any left;
    Any right;

    Any& get(const std::string& member_name) {
        if (member_name == "left") {
            return left;
        }
        else if (member_name == "right") {
            return right;
        }
    }

    Any call(const std::string& member_name, const Array& args, const Dictionary& kwargs) {
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

std::string check_tree_str(Any tree) {
    Any res = check_tree(tree);
    return std::to_string(to_int(res));
}

void free_tree(Any tree) {
    Any left = tree.get("left");
    if (left != Any {}) {
        free_tree(left);
        free_tree(tree.get("right"));
    }
    delete reinterpret_cast<Tree*>(to_ptr(tree));
}

std::string test_trees(Any arg) {
    Any min_depth = 4;
    Any max_depth = arg;
    Any stretch_depth = max_depth + 1;
    Any stretch_tree = make_tree(stretch_depth);
    std::stringstream ss;
    ss << check_tree_str(stretch_tree) << ", ";
//    do_not_optimize(stretch_tree);
//    std::cout << "stretch tree of depth " << stretch_depth << " check: " << check_tree(stretch_tree) << std::endl;
#ifdef FREE
    free_tree(stretch_tree);
#endif
    Any long_lived_tree = make_tree(max_depth);
    Any iterations = pow(2, max_depth);
    for (int64_t depth = min_depth; depth < stretch_depth; depth+=2) {
        Any check = 0;
        for (int64_t i = 0; i < iterations; i++) {
            Any temp_tree = make_tree(depth);
            check = check + check_tree(temp_tree);
#ifdef FREE
            free_tree(temp_tree);
#endif
        }
        ss << check << ", ";
//        do_not_optimize(check);
//        std::cout << iterations << " trees of depth " << depth << " check: " << check << std::endl;
        iterations = idiv(iterations, 4);
    }
//    do_not_optimize(long_lived_tree);
    ss << check_tree_str(long_lived_tree);
#ifdef FREE
    free_tree(long_lived_tree);
#endif
    return ss.str();
//    std::cout << "long lived tree of depth " << to_int(max_depth) << " check: " << check_tree(long_lived_tree) << std::endl;
}
