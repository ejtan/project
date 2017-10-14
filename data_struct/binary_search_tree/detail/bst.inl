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


/* recursive_inorder_print()
 * Recursively prints values in sorted order.
 */
template <typename T>
void BST<T>::recursive_inorder_print(const BST_Node<T> *node) const
{
    if (node->left)
        recursive_inorder_print(node->left);

    std::cout << node->data << ' ';

    if (node->right)
        recursive_inorder_print(node->right);
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


/* max()
 * Transverses the tree to the max value.
 */
template <typename T>
T BST<T>::max() const
{
    // TODO: Add in handeling for when root is not allocated.
    BST_Node<T> *node = root;

    while (node->right)
        node = node->right;

    return node->data;
}


/* min()
 * Transverses the tree to the min value.
 */
template <typename T>
T BST<T>::min() const
{
    // TODO: Add in handeling for when root is not allocated.
    BST_Node<T> *node = root;

    while (node->left)
        node = node->left;

    return node->data;
}


/* inorder_print()
 * Prints data in sorted order.
 */
template <typename T>
void BST<T>::inorder_print() const
{
    recursive_inorder_print(root);
    std::cout << std::endl;
}

