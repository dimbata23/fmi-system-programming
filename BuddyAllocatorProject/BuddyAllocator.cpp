#include <cstdlib>
#include <cmath>
#include "BuddyAllocator.hpp"

constexpr const size_t BYTE_SIZE = 1;
constexpr const size_t BITS_IN_BYTE = 8;
constexpr const size_t MIN_BLOCK_POWER = 2; // TODO: Replace with a higher number ( 2^5 ... 2^7 )
const size_t BuddyAllocator::MIN_BLOCK_SIZE = 1 << MIN_BLOCK_POWER; // Min block size is a power of 2

// Alloc func used to allocate the memory block
inline char* alloc( size_t bytesCount )
{
    return static_cast<char*>( calloc( bytesCount, BYTE_SIZE ) );
}

// Alloc func used to free the memory block
inline void dealloc( void* address )
{
    free( address );
}

// Get next power of 2 >= n
size_t nextPowerOf2( size_t n )
{
    size_t result = 1;

    while ( n >>= 1 )
        result <<= 1;

    return result <<= 1;
}


BuddyAllocator::BuddyAllocator( size_t initSize )
    : ftotalAllocs( 0 )
    , ftotalAllocSize( 0 )
    , fcurrAllocs( 0 )
    , fcurrAllocSize( 0 )
    , fmemoryBlockSize ( initSize )
    , fdata( alloc( fmemoryBlockSize ) )
{}


bool BuddyAllocator::isSplit(size_t index) const
{
    // fdata is represented as [ [split table][free table][data] ]
    size_t byte = index / BITS_IN_BYTE;
    size_t offset = index % BITS_IN_BYTE;
    unsigned char mask = ( 1 << ( BITS_IN_BYTE - 1 ) ) >> offset;
    return fdata[ byte ] & mask;
}


bool BuddyAllocator::isFree(size_t index) const
{
    // fdata is represented as [ [split table][free table][data] ]
    size_t byte = index / BITS_IN_BYTE;
    size_t offset = index % BITS_IN_BYTE;
    unsigned char mask = ( 1 << ( BITS_IN_BYTE - 1 ) ) >> offset;
    return fstartOfFreeTable[ byte ] & mask;
}


void BuddyAllocator::setSplit( size_t index, bool split = true )
{
    // fdata is represented as [ [split table][free table][data] ]
    size_t byte = index / BITS_IN_BYTE;
    size_t offset = index % BITS_IN_BYTE;
    unsigned char mask = ( 1 << ( BITS_IN_BYTE - 1 ) ) >> offset;
    if ( split )
        fdata[ byte ] |= mask;
    else
        fdata[ byte ] &= ~mask;
}


void BuddyAllocator::setFree( size_t index, bool free = true )
{
    // fdata is represented as [ [split table][free table][data] ]
    size_t byte = index / BITS_IN_BYTE;
    size_t offset = index % BITS_IN_BYTE;
    unsigned char mask = ( 1 << ( BITS_IN_BYTE - 1 ) ) >> offset;
    if ( free )
        fstartOfFreeTable[ byte ] |= mask;
    else
        fstartOfFreeTable[ byte ] &= ~mask;
}


inline size_t BuddyAllocator::layerBlockSize(size_t layerIndex) const
{
    return MIN_BLOCK_SIZE << ( flayersCount - layerIndex - 1 );
}


BuddyAllocator::~BuddyAllocator()
{
    dealloc( fdata );
}


BuddyAllocator& BuddyAllocator::getInstance()
{
    static BuddyAllocator finstance;
    return finstance;
}


void BuddyAllocator::deallocate( void* pToFree )
{
    // TODO: 
}


void BuddyAllocator::initMemoryBlock( size_t size )
{
    dealloc ( fdata );
    fmemoryBlockSize = size;
    fdata = alloc( fmemoryBlockSize );

    if ( fdata ) {
        size_t np2 = nextPowerOf2( size );
        size_t leavesCount = np2 / MIN_BLOCK_SIZE;
        size_t blocksCount = 2 * leavesCount - 1;
        size_t freeTableSize = ceil( (2 * leavesCount - 1) / static_cast<double>( BITS_IN_BYTE ) );
        size_t splitTableSize = ceil((leavesCount - 1) / static_cast<double>( BITS_IN_BYTE ));
        size_t metaDataSize = freeTableSize + splitTableSize;
        flayersCount = static_cast< size_t >( log2( blocksCount ) );
        fstartOfFreeTable = fdata + splitTableSize;
        fstartOfData = fdata + (size - np2);
        long long bytesToBeMarkedAsUsed = metaDataSize + ( np2 - size );

        // Mark metadata as usedup memory
        if ( bytesToBeMarkedAsUsed > 0 ) {
            ffreeMemory -= bytesToBeMarkedAsUsed;
            size_t currLayer = 0;
            size_t currIndex = 0;
            while ( currLayer < flayersCount - 1 && bytesToBeMarkedAsUsed < layerBlockSize( currLayer + 1 ) ) {
                currIndex = ( 1 << currLayer ) - 1;
                setSplit( currIndex );
                ++currLayer;
            }
            currIndex = ( 1 << currLayer ) - 1;
            setFree( currIndex, false );
            ffreeMemory = fmemoryBlockSize - layerBlockSize( currLayer );
        }
    } else {
        ffreeMemory = 0;
    }
}
