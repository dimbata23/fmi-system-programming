#include <iostream>
#include <vector>
#include "BuddyAllocator.hpp"

int main()
{
    std::vector< int, StlBuddyAllocator<int> > vec;



    return 0;
}
