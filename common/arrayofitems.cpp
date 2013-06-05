#include "arrayofitems.h"
#include <assert.h>
#include <memory.h>


#define USE_FAST_SETITEM 1


//----------------------------------------------
ArrayOfItems::ArrayOfItems(uint32_t arraySize)
{
    // Initialize cells
    m_arraySize = arraySize;
    m_entries = new Entry[arraySize];
    Clear();
}


//----------------------------------------------
ArrayOfItems::~ArrayOfItems()
{
    // Delete cells
    delete[] m_entries;
}


//----------------------------------------------
#if USE_FAST_SETITEM
void ArrayOfItems::SetItem(uint32_t key, uint32_t value)
{
    assert(key != 0);
    assert(value != 0);

    for (uint32_t idx = 0;; idx++)
    {
        // Load the key that was there.
        uint32_t probedKey = mint_load_32_relaxed(&m_entries[idx].key);
        if (probedKey != key)
        {
            // The entry was either free, or contains another key.
            if (probedKey != 0)
                continue;           // Usually, it contains another key. Keep probing.
                
            // The entry was free. Now let's try to take it using a CAS.
            uint32_t prevKey = mint_compare_exchange_strong_32_relaxed(&m_entries[idx].key, 0, key);
            if ((prevKey != 0) && (prevKey != key))
                continue;       // Another thread just stole it from underneath us.

            // Either we just added the key, or another thread did.
        }
        
        // Store the value in this array entry.
        mint_store_32_relaxed(&m_entries[idx].value, value);
        return;
    }
}
#else
void ArrayOfItems::SetItem(uint32_t key, uint32_t value)
{
    for (uint32_t idx = 0;; idx++)
    {
        uint32_t prevKey = mint_compare_exchange_strong_32_relaxed(&m_entries[idx].key, 0, key);
        if ((prevKey == 0) || (prevKey == key))
        {
            mint_store_32_relaxed(&m_entries[idx].value, value);
            return;
        }
    }
}
#endif

//----------------------------------------------
uint32_t ArrayOfItems::GetItem(uint32_t key)
{
    assert(key != 0);

    for (uint32_t idx = 0;; idx++)
    {
        uint32_t probedKey = mint_load_32_relaxed(&m_entries[idx].key);
        if (probedKey == key)
            return mint_load_32_relaxed(&m_entries[idx].value);
        if (probedKey == 0)
            return 0;          
    }
}


//----------------------------------------------
uint32_t ArrayOfItems::GetItemCount()
{
    for (uint32_t idx = 0;; idx++)
    {
        uint32_t probedKey = mint_load_32_relaxed(&m_entries[idx].key);
        if (probedKey == 0)
            return idx;
    }
}


//----------------------------------------------
void ArrayOfItems::Clear()
{
    memset(m_entries, 0, sizeof(Entry) * m_arraySize);
}
