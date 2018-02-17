#pragma once


#include <iostream>
#include <initializer_list>
#include "list_node.h"


/* Forward Declaration
 */
template <typename T, int NodeSize> class Unrolled_List;
template <typename T, int NodeSize>
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
template <typename T, int NodeSize = 8>
class Unrolled_List
{
    private: // Variables
        List_Node<T, NodeSize> *head, *tail;
        int N;

    private: // Methods
        void split_node(List_Node<T, NodeSize> *ptr);

    public: // Methods
        // Constructors and destructors.
        Unrolled_List();
        template <class InputIt> Unrolled_List(InputIt begin, InputIt end);
        Unrolled_List(int cnt);
        Unrolled_List(std::initializer_list<T> lst);
        Unrolled_List(const Unrolled_List &rhs);
        Unrolled_List(Unrolled_List &&rhs);
        ~Unrolled_List();

        // Capacity
        bool empty() const noexcept;
        int size() const noexcept;

        // Access
        T& back();
        const T& back() const;
        T& front();
        const T& front() const;

        // Modifiers
        void clear() noexcept;
        void insert(int pos, const T &item);
        void insert(int pos, T &&item);
        void push_back(const T &item);
        void push_back(T &&item);
        void push_front(const T &item);
        void push_front(T &&item);

        // Overloaded operators
        Unrolled_List& operator=(const Unrolled_List &rhs);
        Unrolled_List& operator=(Unrolled_List &&rhs);
        friend std::ostream& operator<< <T, NodeSize>(std::ostream &os, const Unrolled_List &rhs);
};


#include "detail/unrolled_linked_list.cpp"
