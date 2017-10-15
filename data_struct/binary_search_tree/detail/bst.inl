#include <iostream>


/*-------------------------------------------------------------------------------------------------
 * PRIVATE METHODS
 *-----------------------------------------------------------------------------------------------*/

/* delete_tree()
 * Deletes nodes in the BST recursively.
 */
template <typename Key, typename T>
void BST<Key, T>::delete_tree(BST_Node<Key, T> *node)
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
template <typename Key, typename T>
void BST<Key, T>::recursive_inorder_print(const BST_Node<Key, T> *node) const
{
    if (node->left)
        recursive_inorder_print(node->left);

    std::cout << node->key << ' ';

    if (node->right)
        recursive_inorder_print(node->right);
}


/* recursive_remove()
 * Recursively removes value form a tree.
 */
template <typename Key, typename T>
void BST<Key, T>::recursive_remove(const Key &key, BST_Node<Key, T> *&node)
{
    if (!node) {
        std::cout << key << " is not in the tree. Nothing is removed.\n";
        return;
    }

    if (key < node->key) {
        recursive_remove(key, node->left);
    } else if (key > node->key) {
        recursive_remove(key, node->right);
    } else if (node->left && node->right) {
        // Case for 2 children
        node->key = min_node(node->right)->key;
        recursive_remove(node->key, node->right);
    } else {
        BST_Node<Key, T> *old_node = node;
        node = (node->left) ? node->left : node->right;
        delete old_node;

        n_ele--;
    }
}


/* min_node()
 * Takes in a node as root and finds the min node of the subtree.
 */
template <typename Key, typename T>
BST_Node<Key, T>* BST<Key, T>::min_node(BST_Node<Key, T> *node) const
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
template <typename Key, typename T>
BST<Key, T>::BST()
{
    root = nullptr;
    n_ele = 0;
}


/* Destructor
 */
template <typename Key, typename T>
BST<Key, T>::~BST()
{
    if (root)
        delete_tree(root);
}


/* insert()
 * Inserts an object into the tree. Inserts into root if tree is empty. Otherwise, transverse the
 * tree until a node to insert the value is found. If the value is already inside the tree,
 * a message is printed out and the value is not inserted into the tree.
 */
template <typename Key, typename T>
void BST<Key, T>::insert(const Key &key, const T &value)
{
    if (!root) {
        root = new BST_Node<Key, T>(key, value, nullptr, nullptr);
        n_ele++;
    } else {
        // Use a curr node ptr for exiting while loop. prev ptr will be accessed when allocating
        // new node
        BST_Node<Key, T> *prev, *curr = root;

        while (curr) {
            prev = curr;

            if (key < curr->key) {
                curr = curr->left;
            } else if (key > curr->key) {
                curr = curr->right;
            } else {
                std::cout << key << " is already in tree.\n";
                return;
            }
        }

        // Allocate new node
        if (key < prev->key)
            prev->left = new BST_Node<Key, T>(key, value, nullptr, nullptr);
        else
            prev->right = new BST_Node<Key, T>(key, value, nullptr, nullptr);

        n_ele++;
    }
}


/* remove()
 */
template <typename Key, typename T>
void BST<Key, T>::remove(const Key &key)
{
    recursive_remove(key, root);
}


/* size()
 * Returns the number of allocated elements in the tree.
 */
template <typename Key, typename T>
std::size_t BST<Key, T>::size() const
{
    return n_ele;
}


/* max()
 * Transverses the tree to the max value.
 */
template <typename Key, typename T>
Key BST<Key, T>::max_key() const
{
    // TODO: Add in handeling for when root is not allocated.
    // Consider returing iterator to node rather than value itself.
    BST_Node<Key, T> *node = root;

    while (node->right)
        node = node->right;

    return node->key;
}


/* min()
 * Transverses the tree to the min value.
 */
template <typename Key, typename T>
Key BST<Key, T>::min_key() const
{
    // TODO: Add in handeling for when root is not allocated.
    // Consider returing iterator to node rather than value itself.
    BST_Node<Key, T> *node = root;

    while (node->left)
        node = node->left;

    return node->key;
}


/* contains()
 * Checks if a specific key is inside the tree.
 */
template <typename Key, typename T>
bool BST<Key, T>::contains(const Key &key) const
{
    if (!root) {
        return false;
    } else {
        BST_Node<Key, T> *curr = root;

        while (curr) {
            // If the value is neither less than or greater than the value in the current node,
            // than the value must be the same as the current node.
            if (key < curr->key)
                curr = curr->left;
            else if (key > curr->key)
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
template <typename Key, typename T>
bool BST<Key, T>::isEmpty() const
{
    if (!root)
        return true;
    else
        return false;
}


/* inorder_print()
 * Prints data in sorted order.
 */
template <typename Key, typename T>
void BST<Key, T>::inorder_print() const
{
    recursive_inorder_print(root);
    std::cout << std::endl;
}
