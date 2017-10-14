/* Written by : Eric Tan
 */

#ifndef BST_H
#define BST_H


#include "bst_node.h"


/* Class : BST
 * Binary Tree implamentation. Nodes are based on the BST_Node<T> struct.
 */
template <typename T>
class BST
{
    private:
        BST_Node<T> *root;
        std::size_t n_ele;

        void delete_tree(BST_Node<T> *node);
    public:
        BST();
        ~BST();
        void insert(const T &value);
};


#include "detail/bst.inl"

#endif
