#include <algorithm>


/*------------------------------------------------------------------------------------------------
 * PRIVATE METHODS
 *----------------------------------------------------------------------------------------------*/

/* split_node(ptr)
 *
 * @param : ptr = pointer to the node to split.
 *
 * Within public methods, once the array is filled, a new node is allocated and the data is
 * split between the two.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::split_node(List_Node<T, NodeSize> *ptr)
{
    List_Node<T, NodeSize> *new_node = new List_Node<T, NodeSize>;

    std::move(ptr->data.begin() + (NodeSize / 2), ptr->data.end(), new_node->data.begin());

    ptr->size = NodeSize / 2;
    new_node->size = NodeSize - (NodeSize / 2);

    // new_node is always entered to the right of the pointer. We need a special case for ptr = tail
    if (ptr == tail) {
        tail->next = new_node;
        new_node->prev = tail;
        tail = new_node;
    } else {
        new_node->next = ptr->next;
        ptr->next->prev = new_node;
        new_node->prev = ptr;
        ptr->next = new_node;
    }
}


/*------------------------------------------------------------------------------------------------
 * PUBLIC METHODS
 *----------------------------------------------------------------------------------------------*/

/* Default Constructor
 */
template <typename T, size_t NodeSize>
Unrolled_List<T, NodeSize>::Unrolled_List() : N(0)
{
    head = new List_Node<T, NodeSize>;
    tail = head;
}


/* Destructor
 */
template <typename T, size_t NodeSize>
Unrolled_List<T, NodeSize>::~Unrolled_List()
{
    for (List_Node<T, NodeSize> *ptr = head; ptr->next; ptr = head) {
        head = ptr->next;
        delete ptr;
    } // Loop over allocated links
}


/* insert(pos, item)
 *
 * @param : pos = position to insert the item.
 * @param : item = item to insert
 *
 * Inserts the item into list[pos] and perform the necessary data movement and allocation of
 * nodes if needed.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::insert(size_t pos, const T &item)
{
    List_Node<T, NodeSize> *ptr = head;
    int skip = 0;

    while (skip <= (static_cast<int>(pos) - static_cast<int>(ptr->size))) {
        skip += static_cast<int>(ptr->size);
        ptr = ptr->next;
    } // Skip to the correct node.

    // If skip is the same as pos, we are at the front of the array in the node
    if (skip == pos)
        ptr->push_front(item);
    else
        ptr->insert(pos - static_cast<size_t>(skip), item);

    if (ptr->size == NodeSize)
        split_node(ptr);

    N++;
}


/* push_back(item)
 *
 * @ param : item = Object to insert at the end of the list.
 *
 *  Adds an item to the end of the data array in the tail node. If the node is at capacity,
 *  we split the data array in half and set a new tail node.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::push_back(const T &item)
{
    tail->push_back(item);

    if (tail->size == NodeSize)
        split_node(tail);

    N++;
}


/* push_front(item)
 *
 * @param : item = object to insert at the front of the list.
 *
 * Shifts all data in a node back and inserts item to the front. If the node is full, we allocate
 * a new node, split the data in half, and place the node between head and head->next.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::push_front(const T &item)
{
    head->push_front(item);

    if (head->size == NodeSize)
        split_node(head);

    N++;
}


/* overloaded << operator
 *
 * @param : os = output stream
 * @param : rhs = Unrolled_List<T, NodeSize> to output.
 *
 * Overloads the << opearator to output each element in the list, with a newspace
 * between each element.
 */
template <typename T, size_t NodeSize>
std::ostream& operator<<(std::ostream &os, const Unrolled_List<T, NodeSize> &rhs)
{
    for (List_Node<T, NodeSize> *ptr = rhs.head; ptr; ptr = ptr->next)
        for (int i = 0; i < ptr->size; i++)
            os << ptr->data[i] << '\n';

    return os;
}
