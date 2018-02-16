#pragma once


#include <iostream>
#include <initializer_list>
#include "list_node.h"


/* Forward Declaration
 */
template <typename T, size_t NodeSize> class Unrolled_List;
template <typename T, size_t NodeSize>
std::ostream& operator<<(std::ostream &os, const Unrolled_List<T, NodeSize> &rhs);


/* class Unrolled_List
 *
 * @template : T = type the list will store.
 * @template : NodeSize = number of elements each node stores. Defaults to 64 elements.
 *
 * Unrolled list is a doubly linked list where each node stores NodeSize number of data rather
 * than 1 element. This is done to improve cahce perfromance, but increases overhead in memory and
 * inserting.
 */
template <typename T, size_t NodeSize = 8>
class Unrolled_List
{
    private: // Variables
        List_Node<T, NodeSize> *head, *tail;
        size_t N;

    private: // Methods
        void split_node(List_Node<T, NodeSize> *ptr);

    public: // Methods
        // Constructors and destructors.
        Unrolled_List();
        ~Unrolled_List();

        // Capacity
        bool empty() const noexcept;
        size_t size() const noexcept;

        // Modifiers
        void clear() noexcept;
        void insert(size_t pos, const T &item);
        void insert(size_t pos, T &&item);
        void push_back(const T &item);
        void push_back(T &&item);
        void push_front(const T &item);
        void push_front(T &&item);

        // Overloaded operators
        friend std::ostream& operator<< <T, NodeSize>(std::ostream &os, const Unrolled_List &rhs);
};


#include "detail/unrolled_linked_list.cpp"
