#include "BuddyAllocator.hpp"


template <class T>
BuddyAllocator<T>::BuddyAllocator() noexcept
    : totalAllocs(0)
    , totalAllocSize(0)
{}


template <class T>
typename BuddyAllocator<T>::pointer BuddyAllocator<T>::allocate(size_type objCount)
{
    const BuddyAllocator<T>::size_type total = sizeof(T) * objCount;
    totalAllocSize += total;
    ++totalAllocs;
    return static_cast<T*>(malloc(total));
}


template<class T>
void BuddyAllocator<T>::deallocate(pointer ptr, size_type)
{
    free(ptr);
}
