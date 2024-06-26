/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   contributed by Kevin Carson
*/


#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>


typedef struct tn {
    struct tn*    left;
    struct tn*    right;
} treeNode;


treeNode* NewTreeNode(treeNode* left, treeNode* right)
{
    treeNode*    new_node;

    new_node = (treeNode*)malloc(sizeof(treeNode));

    new_node->left = left;
    new_node->right = right;

    return new_node;
} /* NewTreeNode() */


long ItemCheck(treeNode* tree)
{
    if (tree->left == NULL)
        return 1;
    else
        return 1 + ItemCheck(tree->left) + ItemCheck(tree->right);
} /* ItemCheck() */

std::string ItemCheckStr(treeNode* tree)
{
    long check = ItemCheck(tree);
    return std::to_string(check);
}

treeNode* BottomUpTree(unsigned depth)
{
    if (depth > 0)
        return NewTreeNode
        (
            BottomUpTree(depth - 1),
            BottomUpTree(depth - 1)
        );
    else
        return NewTreeNode(NULL, NULL);
} /* BottomUpTree() */


void DeleteTree(treeNode* tree)
{
    if (tree->left != NULL)
    {
        DeleteTree(tree->left);
        DeleteTree(tree->right);
    }

    free(tree);
} /* DeleteTree() */


std::string test_trees_c(int arg)
{
    unsigned   N, depth, minDepth, maxDepth, stretchDepth;
    treeNode   *stretchTree, *longLivedTree, *tempTree;

    N = arg;

    minDepth = 4;
    std::stringstream ss;

    if ((minDepth + 2) > N)
        maxDepth = minDepth + 2;
    else
        maxDepth = N;

    stretchDepth = maxDepth + 1;

    stretchTree = BottomUpTree(stretchDepth);
    ss << ItemCheckStr(stretchTree) << ", ";

    /* printf */
    /* ( */
    /*     "stretch tree of depth %u\t check: %li\n", */
    /*     stretchDepth, */
    /*     ItemCheck(stretchTree) */
    /* ); */

    DeleteTree(stretchTree);

    longLivedTree = BottomUpTree(maxDepth);

    for (depth = minDepth; depth <= maxDepth; depth += 2)
    {
        long    i, iterations, check;

        iterations = pow(2, maxDepth - depth + minDepth);

        check = 0;

        for (i = 1; i <= iterations; i++)
        {
            tempTree = BottomUpTree(depth);
            check += ItemCheck(tempTree);
            DeleteTree(tempTree);
        } /* for(i = 1...) */
        ss << std::to_string(check) << ", ";

        /* printf */
        /* ( */
        /*     "%li\t trees of depth %u\t check: %li\n", */
        /*     iterations, */
        /*     depth, */
        /*     check */
        /* ); */
    } /* for(depth = minDepth...) */

    /* printf */
    /* ( */
    /*     "long lived tree of depth %u\t check: %li\n", */
    /*     maxDepth, */
    /*     ItemCheck(longLivedTree) */
    /* ); */
    ss << ItemCheckStr(longLivedTree);
    DeleteTree(longLivedTree);
    return ss.str();
} 



//// BENCHMARKING:
//#include <time.h>
//
//int main()
//{
//  clock_t start_time, end_time;
//  double execution_time;
//
//  for (int i = 0; i < 10; i++)
//    {
//      start_time = clock();
//      benchmark(21);
//      end_time = clock();
//
//      execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
//
//      printf("%f\n", execution_time);
//    }
//
//  return 0;
//}

