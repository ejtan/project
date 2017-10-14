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
        void recursive_inorder_print(const BST_Node<T> *node) const;
    public:
        BST();
        ~BST();
        void insert(const T &value);
        std::size_t size() const;
        T max() const;
        void inorder_print() const;
};


#include "detail/bst.inl"

#endif
