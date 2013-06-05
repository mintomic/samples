#ifndef __COMMON_COLLECTIONTEST2_H__
#define __COMMON_COLLECTIONTEST2_H__

#include <mintomic/core.h>

class IExperiment;

#define TEST_LOOKUP_2 1


//--------------------------------------------------------------
//  In this experiment, each thread adds the same set of keys to the
//  collection, but with different values.
//--------------------------------------------------------------
namespace CollectionTest2
{
    struct ItemSet
    {
        uint32_t key;
        uint32_t value[4];
    };

    struct TrialParams
    {
        ItemSet* items;
        uint32_t numItems;
        int numThreads;
    };

    class BasePlugin
    {
    public:
        virtual ~BasePlugin() {}
        virtual void CreateCollection(uint32_t size) = 0;
        virtual bool DoThread(int arg, TrialParams* params) = 0;
        virtual bool CheckCollection(TrialParams* params) = 0;
    };

    //--------------------------------------------------------------
    //  Instantiate this class with the type of collection you want to test (eg. ArrayOfItems).
    //--------------------------------------------------------------
    template <class T> class Plugin : public BasePlugin
    {
    private:
        T* m_collection;

    public:
        Plugin() : m_collection(NULL)
        {
        }

        ~Plugin()
        {
            if (m_collection)
                delete m_collection;
        }

        virtual void CreateCollection(uint32_t size)
        {
            if (m_collection)
                delete m_collection;
            m_collection = new T(size);
        }

        virtual bool DoThread(int threadNum, TrialParams* params)
        {
            for (uint32_t i = 0; i < params->numItems; i++)
            {
                ItemSet* item = &params->items[i];
                m_collection->SetItem(item->key, item->value[threadNum]);
            }

#if TEST_LOOKUP_2
            return CheckCollection(params);
#endif        

            return true;
        }

        virtual bool CheckCollection(TrialParams* params)
        {
            for (uint32_t i = 0; i < params->numItems; i++)
            {
                ItemSet* item = &params->items[i];
                uint32_t value = m_collection->GetItem(item->key);
                int j;
                for (j = 0; j < params->numThreads; j++)
                {
                    if (value == item->value[j])
                        break;
                }
                if (j >= params->numThreads)
                    return false;
            }
            if (m_collection->GetItemCount() != params->numItems)
                return false;
            return true;
        }
    };

    //--------------------------------------------------------------
    //  Return an IExperiment object suitable for passing to LoopForever.
    //--------------------------------------------------------------
    IExperiment* CreateExperiment(BasePlugin* plugin, uint32_t numItems, uint32_t tableSize);
} // namespace CollectionTest2


#endif // __COMMON_COLLECTIONTEST2_H__
