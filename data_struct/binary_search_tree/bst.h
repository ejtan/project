/* Written by : Eric Tan
 */

#ifndef BST_H
#define BST_H


#include "bst_node.h"


/* Class : BST
 * Binary Tree implamentation. Nodes are based on the BST_Node<T> struct.
 */
template <typename Key, typename T>
class BST
{
    private:
        BST_Node<Key, T> *root;
        std::size_t n_ele;

        void delete_tree(BST_Node<Key, T> *node);
        void recursive_inorder_print(const BST_Node<Key, T> *node) const;
        void recursive_remove(const Key &key, BST_Node<Key, T> *&node);
        BST_Node<Key, T>* min_node(BST_Node<Key, T> *node) const;
    public:
        BST();
        ~BST();
        void insert(const Key &key, const T &value);
        void remove(const Key &key);
        std::size_t size() const;
        T max() const;
        T min() const;
        bool contains(const Key &key) const;
        bool isEmpty() const;
        void inorder_print() const;
};


#include "detail/bst.inl"

#endif
