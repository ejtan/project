#include <utility>


/* Default constructor
 */
template <typename T>
BST_Node<T>::BST_Node()
{
    left  = nullptr;
    right = nullptr;
}


/* Constructor
 */
template <typename T>
BST_Node<T>::BST_Node(const T &item, BST_Node<T> *l, BST_Node<T> *r) :
    data(item), left(l), right (r)
{
}


/* Constructor
 */
template <typename T>
BST_Node<T>::BST_Node(T &&item, BST_Node<T> *l, BST_Node<T> *r) :
    data(std::move(item)), left(l), right(r)
{
}
