/* Written by : Eric Tan
 */

#ifndef BST_NODE_H
#define BST_NODE_H


/* struct: BST_Node
 * Struct containing a BST Node. Contains the node and pointers to the left and right child.
 */
template <typename Key, typename T>
struct BST_Node
{
    Key key;
    T data;
    BST_Node<Key, T> *left, *right;

    BST_Node();
    BST_Node(const Key &k, const T &d, BST_Node<Key, T> *l, BST_Node<Key, T> *r);
    BST_Node(Key &&k, T &&d, BST_Node<Key, T> *l, BST_Node<Key, T> *r);
};


#include "detail/bst_node.inl"


#endif
