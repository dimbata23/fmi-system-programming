#pragma once

template <class T>
class BuddyAllocator
{
public:

    // STL Compatibility
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = size_t;

    BuddyAllocator() noexcept;

    template <class U>
    BuddyAllocator(const BuddyAllocator<U>& other) noexcept {}

    pointer allocate(size_type objCount);
    void deallocate(pointer ptr, size_type objCount);

    inline size_type getTotalAllocations() noexcept { return totalAllocs; }
    inline size_type getTotalAllocationSize() noexcept { return totalAllocSize; }

private:
    size_type totalAllocs;
    size_type totalAllocSize;
};

// Including the template method implementations
#include "BuddyAllocator.ipp"
