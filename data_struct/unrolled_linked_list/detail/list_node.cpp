/* Default constructor
 */
template <typename T, size_t N>
List_Node<T, N>::List_Node() : prev(nullptr), next(nullptr), size(0)
{
}


/* push_back(item)
 *
 * @param : item = object to add into the back of the array
 *
 * Inserts an item to the back of the array.
 */
template <typename T, size_t N>
void List_Node<T, N>::push_back(const T &item)
{
    data[size++] = item;
}


/* push_front(item)
 *
 * @param : item = object to add to the front of the array.
 *
 * Shifts all the items in the node back by 1, then places the item to the front of the array.
 */
template <typename T, size_t N>
void List_Node<T, N>::push_front(const T &item)
{
    for (int i = size - 1; i >= 0; i--)
        data[i + 1] = data[i];

    data[0] = item;
    size++;
}


/* insert(item)
 *
 * @param : pos = position in the array to insert the item.
 * @param : item = object to add into the middle of the array
 *
 * Mooves all items whose index >= pos and moves them back 1 space. Then inserts that item into pos.
 */
template <typename T, size_t N>
void List_Node<T, N>::insert(size_t pos, const T &item)
{
    for (int i = size - 1; i >= pos; i--)
        data[i + 1] = data[i];

    data[pos] = item;
    size++;
}
