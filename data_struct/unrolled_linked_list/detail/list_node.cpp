#include <algorithm>


/* Default constructor
 */
template <typename T, int N>
List_Node<T, N>::List_Node() : prev(nullptr), next(nullptr), size(0)
{
}


/* push_back(item)
 *
 * @param : item = object to add into the back of the array
 *
 * Inserts an item to the back of the array.
 */
template <typename T, int N>
void List_Node<T, N>::push_back(const T &item)
{
    data[size++] = item;
}


/* push_back(item)
 *
 * @param : item = object to add into the back of the array
 *
 * Inserts an item to the back of the array.
 */
template <typename T, int N>
void List_Node<T, N>::push_back(T &&item)
{
    data[size++] = item;
}


/* push_front(item)
 *
 * @param : item = object to add to the front of the array.
 *
 * Shifts all the items in the node back by 1, then places the item to the front of the array.
 */
template <typename T, int N>
void List_Node<T, N>::push_front(const T &item)
{
    data[size++] = item;

    // Rotate elements so that the inserted element is now rotated to position 0
    std::rotate(data.begin(), data.begin() + size - 1, data.begin() + size);
}


/* push_front(item)
 *
 * @param : item = Rvalue ref to object to add to the front of the array.
 *
 * Shifts all the items in the node back by 1, then places the item to the front of the array.
 */
template <typename T, int N>
void List_Node<T, N>::push_front(T &&item)
{
    data[size++] = item;

    // Rotate elements so that the inserted element is now rotated to position 0
    std::rotate(data.begin(), data.begin() + size - 1, data.begin() + size);
}


/* insert(item)
 *
 * @param : pos = position in the array to insert the item.
 * @param : item = object to add into the middle of the array
 *
 * Mooves all items whose index >= pos and moves them back 1 space. Then inserts that item into pos.
 */
template <typename T, int N>
void List_Node<T, N>::insert(int pos, const T &item)
{
    data[size++] = item;

    // Rotate data[pos, size) such that data[size-1] is now at data[pos]
    std::rotate(data.begin() + pos, data.begin() + size - 1, data.begin() + size);
}


/* insert(item)
 *
 * @param : pos = position in the array to insert the item.
 * @param : item = Rvalue reference of object to add into the middle of the array
 *
 * Mooves all items whose index >= pos and moves them back 1 space. Then inserts that item into pos.
 */
template <typename T, int N>
void List_Node<T, N>::insert(int pos, T &&item)
{
    data[size++] = item;

    // Rotate data[pos, size) such that data[size-1] is now at data[pos]
    std::rotate(data.begin() + pos, data.begin() + size - 1, data.begin() + size);
}
