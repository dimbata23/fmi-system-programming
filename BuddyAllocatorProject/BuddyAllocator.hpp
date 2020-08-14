#pragma once

class BuddyAllocator
{
public:
    BuddyAllocator( const BuddyAllocator& )             = delete;
    BuddyAllocator& operator=( const BuddyAllocator& )  = delete;
    ~BuddyAllocator();
    static BuddyAllocator& getInstance();

    template <class T>
    T* allocate( size_t count );

    void deallocate( void* ptr );

    inline size_t   getTotalAllocations() const       { return ftotalAllocs; }
    inline size_t   getTotalAllocationSize() const    { return ftotalAllocSize; }
    inline size_t   getCurrentAllocations() const     { return fcurrAllocs; }
    inline size_t   getCurrentAllocationSize() const  { return fcurrAllocSize; }

    // Sets the size and initializes the memory block used by the allocator
    // WARNING: Any previously allocated data will be lost!
    void initMemoryBlock( size_t size );

    // Sets size and initializes the memory block used by the allocator
    // keeping all previously allocated data.
    // TODO: void reinitMemoryBlock( size_t size );

private:
    BuddyAllocator( size_t initSize = 0 );
    inline size_t leftChildIndex( size_t parentIndex ) const { return 2 * parentIndex + 1; }
    inline size_t rightChildIndex( size_t parentIndex ) const { return 2 * parentIndex + 2; }
    bool isSplit( size_t index ) const;
    bool isFree( size_t index ) const;
    void setSplit( size_t index, bool split = true );
    void setFree( size_t index, bool free = true );
    inline size_t layerBlockSize ( size_t layerIndex ) const;

private:
    size_t  ftotalAllocs;
    size_t  ftotalAllocSize;
    size_t  fcurrAllocs;
    size_t  fcurrAllocSize;
    size_t  fmemoryBlockSize;
    size_t  flayersCount;
    size_t  ffreeMemory;
    char*   fstartOfData;
    char*   fstartOfFreeTable;
    char*   fdata;

private:
    static const size_t MIN_BLOCK_SIZE;
};


template <class T>
T* BuddyAllocator::allocate( size_t count )
{
    const size_t bytes = count * sizeof(T);
    T* result = nullptr;

    if ( bytes < MIN_BLOCK_SIZE ) {
        result = static_cast<T*>( malloc( bytes ) ); // TODO: Replace with a small object allocator / slab
    } else {

        // TODO: find a suitable place

    }

    return result;
}


// STL Compatible wrapper for the Buddy Allocator
template <class T>
class StlBuddyAllocator
{
public:
    // STL Compatibility
    using value_type    = T;
    using pointer       = T*;
    using const_pointer = const T*;

    StlBuddyAllocator() = default;

    template <class U>
    StlBuddyAllocator( const StlBuddyAllocator<U>& other ) {}

    inline pointer allocate( size_t objCount ) const
    {
        return BuddyAllocator::getInstance().allocate<T>( objCount );
    }

    inline void deallocate( pointer ptr, size_t ) const
    {
        BuddyAllocator::getInstance().deallocate( ptr );
    }
};
