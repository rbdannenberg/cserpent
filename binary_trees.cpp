#include <any.h>
#include <op_overload.h>
#include <data_structures/array.h>
#include <data_structures/dictionary.h>
#include <builtin_functions/builtin_functions.h>

class Tree : public Obj {
    Any left;
    Any right;
}
Any make_tree(Any depth
{
    if (depth == 0)
    {
        return *(new Tree {});
    }
    depth = depth - 1;
    Any new_tree = *(new Tree {});
    new_tree.get("left") = make_tree(depth);
    new_tree.get("right") = make_tree(depth);
    return new_tree;
}
Any check_tree(Any node
{
    if (node.get("left") == nil)
    {
        return 1;
    }
    return 1 + check_tree(node.get("left")) + check_tree(node.get("right"));
}
