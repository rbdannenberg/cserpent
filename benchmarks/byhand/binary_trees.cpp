#include <sstream>
#include "any.h"
#include "op_overload.h"
#include "array.h"
#include "dict.h"
#include "builtin_functions.h"
#include "benchmarking_utils.h"

extern Cs_class * Tree_class;

class Tree : public Obj {
public:
    Any more_slots[2];
    Tree() : Obj {Tree_class} {}
    Tree(Any left, Any right) : Obj {Tree_class} {
        set_left(left);
        set_right(right);
    }

    Any get_left() {
        return slots[1];
    }
    Any get_right() {
        return slots[2];
    }
    void set_left(Any x) {
        set_slot(1, x);
    }
    void set_right(Any x) {
        set_slot(2, x);
    }
};

Any Tree_get_left(Obj* self, Array *args, Dict& kwargs) {
    check_dispatch(to_symbol(symbol_get_left), args, kwargs, 0, 0);
    return reinterpret_cast<Tree*>(self)->get_left();
}
Any Tree_get_right(Obj* self, Array *args, Dict& kwargs) {
    check_dispatch(to_symbol(symbol_get_right), args, kwargs, 0, 0);
    return reinterpret_cast<Tree*>(self)->get_right();
}
Any Tree_set_left(Obj* self, Array *args, Dict& kwargs) {
    check_dispatch(to_symbol(symbol_set_left), args, kwargs, 1, 0);
    reinterpret_cast<Tree*>(self)->set_left(args[0]);
    return nil;
}
Any Tree_set_right(Obj* self, Array *args, Dict& kwargs) {
    check_dispatch(to_symbol(symbol_set_right), args, kwargs, 1, 0);
    reinterpret_cast<Tree*>(self)->set_right(args[0]);
    return nil;
}
MemberTable Tree_table {
        {Symbol{"get_left"}, Tree_get_left},
        {Symbol{"get_right"}, Tree_get_right},
        {Symbol{"set_left"}, Tree_set_left},
        {Symbol{"set_right"}, Tree_set_right},
};

Cs_class * Tree_class = new Cs_class {Symbol {"Tree"}, 2, 0b0, &Tree_table};

Any make_tree(Any depth) {
    if (depth == 0) {
        return *(new Tree {});
    }
    depth = depth - 1;
    Any new_tree = *(new Tree {});

    new_tree.call("set_left", Array {make_tree(depth)}, empty_dict);
    new_tree.call("set_right", Array {make_tree(depth)}, empty_dict);
//    new_tree.call("set_left", Array {5}, empty_dict);
//    new_tree.call("set_right", Array {7}, empty_dict);

    return new_tree;
}

Any check_tree(Any tree) {
//    Any comp = tree.call("get_left", empty_array, empty_dict);
//    if (comp == nil) {
//        return 1;
//    }
    if (tree.call("get_left", empty_array, empty_dict) == nil) {
        return 1;
    }
    return check_tree(tree.call("get_left", empty_array, empty_dict)) +
           check_tree(tree.call("get_right", empty_array, empty_dict)) + 1;
}

std::string check_tree_str(Any tree) {
    Any res = check_tree(tree);
    return std::to_string(to_int(res));
}

void free_tree(Any tree) {
    Any left = tree.call("get_left", empty_array, empty_dict);
    if (left != nil) {
        free_tree(left);
        free_tree(tree.call("get_right", empty_array, empty_dict));
    }
    delete reinterpret_cast<Tree*>(to_basic_obj(tree));
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
