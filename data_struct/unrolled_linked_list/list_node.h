#pragma once

#include <array>


/* struct List_Node
 *
 * @template : T = type of data.
 * @template : N = number of elements stored in this node.
 *
 * A node for an unrolled linked list.
 */
template <typename T, size_t N>
struct List_Node
{
    std::array<T, N> data;
    List_Node *prev, *next;
    size_t size;

    List_Node();
    void push_back(const T &item);
    void push_front(const T &item);
    void insert(size_t pos, const T &item);
};


#include "detail/list_node.cpp"
