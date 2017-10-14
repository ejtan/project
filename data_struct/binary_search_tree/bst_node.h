/* Written by : Eric Tan
 */

#ifndef BST_NODE_H
#define BST_NODE_H


/* struct: BST_Node
 * Struct containing a BST Node. Contains the node and pointers to the left and right child.
 */
template <typename T>
struct BST_Node
{
    T data;
    BST_Node<T> *left, *right;

    BST_Node();
    BST_Node(const T &item, BST_Node<T> *l, BST_Node<T> *r);
    BST_Node(T &&item, BST_Node<T> *l, BST_Node<T> *r);
};


#include "detail/bst_node.inl"


#endif
