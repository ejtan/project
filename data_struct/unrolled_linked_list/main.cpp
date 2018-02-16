#include <iostream>
#include "unrolled_linked_list.h"


/*-------------------------------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 *-----------------------------------------------------------------------------------------------*/
void test_insert(Unrolled_List<int, 3> &lst);


/*-------------------------------------------------------------------------------------------------
 * MAIN
 *-----------------------------------------------------------------------------------------------*/
int main(void)
{
    Unrolled_List<int, 3> lst;

    if (lst.empty())
        std::cout << "List is currently empty\n";

    test_insert(lst);
}


/*-------------------------------------------------------------------------------------------------
 * FUNCTIONS
 *-----------------------------------------------------------------------------------------------*/
void test_insert(Unrolled_List<int, 3> &lst)
{
    lst.push_back(2);
    lst.push_back(3);
    lst.push_back(4);
    lst.push_back(5);
    lst.push_back(6);
    lst.push_back(7);
    lst.push_back(8);
    lst.push_back(9);
    lst.push_back(10);

    lst.push_front(-4);
    lst.push_front(-5);
    lst.push_front(-6);
    lst.push_front(-7);
    lst.push_front(-8);
    lst.push_front(-9);
    lst.push_front(-10);

    lst.insert(7, -1);
    lst.insert(7, -2);
    lst.insert(7, -3);
    lst.insert(10, 0);
    lst.insert(11, 1);

    std::cout << "Output of inserts:\n";
    std::cout << lst << std::endl;
    if (!lst.empty())
        std::cout << "List size: " << lst.size() << std::endl;
}
