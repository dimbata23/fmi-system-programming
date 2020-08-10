#include <iostream>
#include <vector>
#include "BuddyAllocator.hpp"

template <class T>
inline void printAllocInfo(const std::vector<T, BuddyAllocator<T>>& vec)
{
    std::cout << " -- Alloc info --\n  Alloc count: " << vec.get_allocator().getTotalAllocations()
        << "\n  Alloc size: " << vec.get_allocator().getTotalAllocationSize() << "\n";
}


int main()
{
    std::vector<int, BuddyAllocator<int>> vec;
    const size_t TEST_MAX = 100;

    printAllocInfo(vec);
    std::cout << "\n";

    for (size_t i = 0; i < 100; i++) {
        vec.push_back(i);
        std::cout << vec[i] << " ";
    }

    std::cout << "\n\n";
    printAllocInfo(vec);

    return 0;
}
