#include <algorithm>
#include <stdexcept>
#include <utility>


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


/* Constructor with two iterators
 *
 * @param : begin = starting input iterator
 * @param : end = ending input iterator
 *
 * Copies elements from [begin, end) to the list. Uses push_back to insert items (which means
 * N is initialized to 0 since push_back() incaments N as items insert).
 */
template <typename T, size_t NodeSize>
template <class InputIt>
Unrolled_List<T, NodeSize>::Unrolled_List(InputIt begin, InputIt end) : N(0)
{
    head = new List_Node<T, NodeSize>;
    tail = head;

    // Use an interator loop to insert elements into the list.
    for (auto it = begin; it != end; it++)
        push_back(*it);
}


/* Constructor with size
 *
 * @param : cnt = number of elements to reserve.
 *
 * Reserves cnt elements by initalizing nodes. Nodes will be initialized to size NodeSize / 2.
 * This allows for more random inserts without having to call split_node(), but will
 * incure more memory usage.
 */
template <typename T, size_t NodeSize>
Unrolled_List<T, NodeSize>::Unrolled_List(size_t cnt) : N(cnt)
{
    size_t half = NodeSize / 2;
    size_t n_nodes = cnt / half;

    // Set head node
    head = new List_Node<T, NodeSize>;

    if (!n_nodes) {
        // If cnt < half, set only 1 node
        head->size = cnt;
        tail = head;
    } else {
        List_Node<T, NodeSize> *ptr = head;

        // Set the first one
        ptr->size = half;

        // Set Nodes (first one is already filled)
        for (size_t i = 1; i < n_nodes; i++) {
            ptr->next =  new List_Node<T, NodeSize>;
            ptr = ptr->next;
            ptr->size = half;
        }

        // Allocate one last node if cnt % half != 0
        if (cnt % half) {
            ptr->next = new List_Node<T, NodeSize>;
            ptr = ptr->next;
            ptr->size = cnt % half;
        }

        tail = ptr;
    }
}


/* Initalizer list constructor
 *
 * @param : lst = Initalizer list input
 *
 * Allocates a node and performs push back operations on each element of the initializer list.
 * We use push_back(), so we must initialize N to 0.
 */
template <typename T, size_t NodeSize>
Unrolled_List<T, NodeSize>::Unrolled_List(std::initializer_list<T> lst) : N(0)
{
    head = new List_Node<T, NodeSize>;
    tail = head;

    // Use push back over all the elements in the initalizer list.
    for (const auto &ele : lst)
        push_back(ele);
}


/* Copy constructor
 *
 * @param : rhs = lvalue ref to input Unrolled_List
 *
 * Performs a copy of rhs, maintaining the size of the nodes.
 */
template <typename T, size_t NodeSize>
Unrolled_List<T, NodeSize>::Unrolled_List(const Unrolled_List &rhs) : N(rhs.N)
{
    head = new List_Node<T, NodeSize>;

    List_Node<T, NodeSize> *prev, *curr = head;

    for (List_Node<T, NodeSize> *rptr = rhs.head; rptr; rptr = rptr->next) {
        // Copy data
        curr->data = rptr->data;
        curr->size = rptr->size;

        // Set link (if ptr != head)
        if (curr != head)
            curr->prev = prev;

        // Set next node if rptr->next exists
        if (rptr->next) {
            curr->next = new List_Node<T, NodeSize>;
            prev = curr;
            curr = curr->next;
        }
    }

    tail = curr;
}


/* Move constructor
 *
 * @param : rhs = rvalue ref to Unrolled_List
 *
 * Performs a shallow copy by swaping newly constructed nodes with rhs.
 */
template <typename T, size_t NodeSize>
Unrolled_List<T, NodeSize>::Unrolled_List(Unrolled_List &&rhs) : N(rhs.N)
{
    head = new List_Node<T, NodeSize>;
    tail = head;

    // Swap head and tail pointers
    std::swap(head, rhs.head);
    std::swap(tail, rhs.tail);

    // Reset rhs list size
    rhs.N = 0;
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


/* empty()
 *
 * @return : Returns true if empty list (N == 0) and false otherwise.
 */
template <typename T, size_t NodeSize>
bool Unrolled_List<T, NodeSize>::empty() const noexcept
{
    return N == 0 ? true : false;
}


/* size()
 *
 * @return : size of the list
 */
template <typename T, size_t NodeSize>
size_t Unrolled_List<T, NodeSize>::size() const noexcept
{
    return N;
}


/* back()
 *
 * @return : Returns a reference to the last item in the list.
 */
template <typename T, size_t NodeSize>
T& Unrolled_List<T, NodeSize>::back()
{
    if (N == 0)
        throw std::out_of_range("Error: Accessing element in empty list.");

    return tail->data[tail->size - 1];
}


/* back()
 *
 * @return : const reference to the last item of the list.
 */
template <typename T, size_t NodeSize>
const T& Unrolled_List<T, NodeSize>::back() const
{
    if (N == 0)
        throw std::out_of_range("Error: Accessing element in empty list.");

    return tail->data[tail->size - 1];
}


/* front()
 *
 * @return : reference to the first item in the list.
 */
template <typename T, size_t NodeSize>
T& Unrolled_List<T, NodeSize>::front()
{
    if (N == 0)
        throw std::out_of_range("Error: Accessing element in empty list.");

    return head->data[0];
}


/* front()
 *
 * @return : reference to the first item in the list.
 */
template <typename T, size_t NodeSize>
const T& Unrolled_List<T, NodeSize>::front() const
{
    if (N == 0)
        throw std::out_of_range("Error: Accessing element in empty list.");

    return head->data[0];
}


/* clear()
 *
 * Clears the linked list. Deletes nodes except for head and sets tail to the head.
 * Resets the size of the node and list.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::clear() noexcept
{
    if (head->next) {
        List_Node<T, NodeSize> *curr, *ptr = head->next;

        // Delete all nodes except head
        while (ptr) {
            curr = ptr;
            ptr = ptr->next;
            delete curr;
        }

        // Only head exists, so set head to tail
        tail = head;
    }

    head->size = 0;
    tail = head;
    N = 0;
}


/* insert(pos, item)
 *
 * @param : pos = position to insert the item.
 * @param : item = item to insert
 *
 * Inserts the item in list[pos]. Elements from list[pos] to the end of the list are moved back.
 * perform the necessary data movement and allocation of nodes if needed.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::insert(size_t pos, const T &item)
{
    if (pos > N)
        throw std::out_of_range("Error: Attempting to insert an element out range.");

    List_Node<T, NodeSize> *ptr = head;
    size_t ele_skip = 0;

    // Loop to skip elements until the correct node is reached.
    while (ele_skip + ptr->size < pos) {
        ele_skip += ptr->size;
        ptr = ptr->next;
    }

    ptr->insert(pos - ele_skip, item);

    if (ptr->size == NodeSize)
        split_node(ptr);

    N++;
}


/* insert(pos, item)
 *
 * @param : pos = position to insert the item.
 * @param : item = Rvalue reference of item to insert
 *
 * Inserts the item in list[pos]. Elements from list[pos] to the end of the list are moved back.
 * perform the necessary data movement and allocation of nodes if needed.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::insert(size_t pos, T &&item)
{
    if (pos > N)
        throw std::out_of_range("Error: Attempting to insert an element out range.");

    List_Node<T, NodeSize> *ptr = head;
    size_t ele_skip = 0;

    // Loop to skip elements until the correct node is reached.
    while (ele_skip + ptr->size < pos) {
        ele_skip += ptr->size;
        ptr = ptr->next;
    }

    ptr->insert(pos - ele_skip, std::move(item));

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


/* push_back(item)
 *
 * @ param : item = Rvalue ref of Object to insert at the end of the list.
 *
 *  Adds an item to the end of the data array in the tail node. If the node is at capacity,
 *  we split the data array in half and set a new tail node.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::push_back(T &&item)
{
    tail->push_back(std::move(item));

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


/* push_front(item)
 *
 * @param : item = Rvalue ref of object to insert at the front of the list.
 *
 * Shifts all data in a node back and inserts item to the front. If the node is full, we allocate
 * a new node, split the data in half, and place the node between head and head->next.
 */
template <typename T, size_t NodeSize>
void Unrolled_List<T, NodeSize>::push_front(T &&item)
{
    head->push_front(std::move(item));

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
