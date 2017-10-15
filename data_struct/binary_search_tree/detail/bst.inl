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


/* recursive_remove()
 * Recursively removes value form a tree.
 */
template <typename T>
void BST<T>::recursive_remove(const T &value, BST_Node<T> *&node)
{
    if (!node) {
        std::cout << value << " is not in the tree. Nothing is removed.\n";
        return;
    }

    if (value < node->data) {
        recursive_remove(value, node->left);
    } else if (value > node->data) {
        recursive_remove(value, node->right);
    } else if (node->left && node->right) {
        // Case for 2 children
        node->data = min_node(node->right)->data;
        recursive_remove(node->data, node->right);
    } else {
        BST_Node<T> *old_node = node;
        node = (node->left) ? node->left : node->right;
        delete old_node;
    }
}


/* min_node()
 * Takes in a node as root and finds the min node of the subtree.
 */
template <typename T>
BST_Node<T>* BST<T>::min_node(BST_Node<T> *node) const
{
    if (!node) {
        return nullptr;
    } else {
        while (node->left)
            node = node->left;
        return node;
    }
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


/* remove()
 */
template <typename T>
void BST<T>::remove(const T &value)
{
    recursive_remove(value, root);
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
    // Consider returing iterator to node rather than value itself.
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
    // Consider returing iterator to node rather than value itself.
    BST_Node<T> *node = root;

    while (node->left)
        node = node->left;

    return node->data;
}


/* contains()
 * Checks if a specific key is inside the tree.
 */
template <typename T>
bool BST<T>::contains(const T &value) const
{
    if (!root) {
        return false;
    } else {
        BST_Node<T> *curr = root;

        while (curr) {
            // If the value is neither less than or greater than the value in the current node,
            // than the value must be the same as the current node.
            if (value < curr->data)
                curr = curr->left;
            else if (value > curr->data)
                curr = curr->right;
            else
                return true;
        }

        return false;
    }
}


/* isEmpty()
 * Checks if the tree is empty.
 */
template <typename T>
bool BST<T>::isEmpty() const
{
    if (!root)
        return true;
    else
        return false;
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
