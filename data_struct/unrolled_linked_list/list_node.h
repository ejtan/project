#pragma once

#include <array>


/* struct List_Node
 *
 * @template : T = type of data.
 * @template : N = number of elements stored in this node.
 *
 * A node for an unrolled linked list.
 */
template <typename T, int N>
struct List_Node
{
    std::array<T, N> data;
    List_Node *prev, *next;
    int size;

    List_Node();
    void push_back(const T &item);
    void push_back(T &&item);
    void push_front(const T &item);
    void push_front(T &&item);
    void insert(int pos, const T &item);
    void insert(int pos, T &&item);
};


#include "detail/list_node.cpp"
