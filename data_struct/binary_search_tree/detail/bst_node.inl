#include <utility>


/* Default constructor
 */
template <typename Key, typename T>
BST_Node<Key, T>::BST_Node()
{
    left  = nullptr;
    right = nullptr;
}


/* Constructor
 */
template <typename Key, typename T>
BST_Node<Key, T>::BST_Node(const Key &k, const T &d, BST_Node<Key, T> *l, BST_Node<Key, T> *r) :
    key(k), data(d), left(l), right(r)
{
}


/* Constructor
 */
template <typename Key, typename T>
BST_Node<Key, T>::BST_Node(Key &&k, T &&d, BST_Node<Key, T> *l, BST_Node<Key, T> *r) :
    key(std::move(k)), data(std::move(d)), left(l), right(r)
{
}
