#include <iostream>
#include "bst.h"

int main(void)
{
    BST<int, double> tree;

    tree.insert(10, 0.0);
    tree.insert(0, 10.0);
    tree.insert(20, 20.0);
    tree.insert(15, 30.0);
    tree.insert(5, 40.0);
    tree.insert(-5, 50.0);
    tree.insert(25, 60.0);

    std::cout << "Number of elements in the tree: " << tree.size() << std::endl;
    std::cout << "Max value: " << tree.max_key() << std::endl;
    std::cout << "Min value: " << tree.min_key() << std::endl;

    tree.inorder_print();

    if (tree.contains(10))
        std::cout << "10 is in the tree.\n";
    if (tree.contains(1000))
        std::cout << "1000 is in the tree.\n";

    std::cout << "\nRemoving 0 from tree.\n";
    tree.remove(0);
    tree.inorder_print();

    std::cout << "Reomving 10 from tree.\n";
    tree.remove(10);
    tree.inorder_print();

    std::cout << "Removing nonexistant node.\n";
    tree.remove(-1000);
    tree.inorder_print();


    //tree.print_tree();

}
