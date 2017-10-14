#include <iostream>


/*-------------------------------------------------------------------------------------------------
 * PRIVATE METHODS
 *-----------------------------------------------------------------------------------------------*/

/* delete_tree()
 * Deletes nodes in the BST recursively.
 */
template <typename T>
void BST<T>::delete_tree(BST_Node<T> *node)
{
    if (node->right)
        delete_tree(node->right);
    if (node->left)
        delete_tree(node->left);

    delete node;
}


/*-------------------------------------------------------------------------------------------------
 * PUBLIC METHODS
 *-----------------------------------------------------------------------------------------------*/

/* Default Constructor
 */
template <typename T>
BST<T>::BST()
{
    root = nullptr;
    n_ele = 0;
}


/* Destructor
 */
template <typename T>
BST<T>::~BST()
{
    if (root)
        delete_tree(root);
}


/* insert()
 * Inserts an object into the tree. Inserts into root if tree is empty. Otherwise, transverse the
 * tree until a node to insert the value is found. If the value is already inside the tree,
 * a message is printed out and the value is not inserted into the tree.
 */
template <typename T>
void BST<T>::insert(const T &value)
{
    if (!root) {
        root = new BST_Node<T>(value, nullptr, nullptr);
        n_ele++;
    } else {
        // Use a curr node ptr for exiting while loop. prev ptr will be accessed when allocating
        // new node
        BST_Node<T> *prev, *curr = root;

        while (curr) {
            prev = curr;

            if (value < curr->data) {
                curr = curr->left;
            } else if (value > curr->data) {
                curr = curr->right;
            } else {
                std::cout << value << " is already in tree.\n";
                return;
            }
        }

        // Allocate new node
        if (value < prev->data)
            prev->left = new BST_Node<T>(value, nullptr, nullptr);
        else
            prev->right = new BST_Node<T>(value, nullptr, nullptr);

        n_ele++;
    }
}


/* size()
 * Returns the number of allocated elements in the tree.
 */
template <typename T>
std::size_t BST<T>::size() const
{
    return n_ele;
}
