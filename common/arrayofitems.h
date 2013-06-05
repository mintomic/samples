#ifndef __COMMON_ARRAYOFITEMS_H__
#define __COMMON_ARRAYOFITEMS_H__

#include <mintomic/mintomic.h>


//----------------------------------------------
//  ArrayOfItems
//
//  Maps 32-bit integers to 32-bit integers.
//  You can call SetItem and GetItem from several threads simultaneously.
//  Both operations are implemented using linear search.
//  SetItem is lock-free.
//  GetItem is wait-free.
//  You can't pass 0 as a key or a value to SetItem.
//  In the m_entries array, key = 0 is reserved to indicate an unused entry.
//  The array must be preallocated to a large enough size ahead of time.
//  You can Clear the array, but only at a time when there are no other calls being made from other threads.
//----------------------------------------------
class ArrayOfItems
{
public:
    struct Entry
    {
        mint_atomic32_t key;
        mint_atomic32_t value;
    };
    
private:    
    Entry* m_entries;
    uint32_t m_arraySize;
    
public:
    ArrayOfItems(uint32_t arraySize);
    ~ArrayOfItems();

    // Basic operations
    void SetItem(uint32_t key, uint32_t value);
    uint32_t GetItem(uint32_t key);
    uint32_t GetItemCount();
    void Clear();
};


#endif // __COMMON_ARRAYOFITEMS_H__
