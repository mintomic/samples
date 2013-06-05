#ifndef __COMMON_COLLECTIONTEST1_H__
#define __COMMON_COLLECTIONTEST1_H__

#include <mintomic/core.h>

class IExperiment;

#define TEST_LOOKUP_1 1


//--------------------------------------------------------------
//  In this experiment, each thread adds a different list of items to the
//  collection. No two threads add the same key.
//--------------------------------------------------------------
namespace CollectionTest1
{
    struct Pair
    {
        uint32_t key;
        uint32_t value;
    };

    struct TrialParams
    {
        Pair* pairs;
        uint32_t numPairs;
        int numThreads;
    };

    class BasePlugin
    {
    public:
        virtual ~BasePlugin() {}
        virtual void CreateCollection(uint32_t size) = 0;
        virtual bool DoThread(int threadNum, TrialParams* params) = 0;
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
            uint32_t lo = params->numPairs * threadNum / params->numThreads;
            uint32_t hi = params->numPairs * (threadNum + 1) / params->numThreads;
        
            for (uint32_t i = lo; i < hi; i++)
            {
                Pair* pair = &params->pairs[i];
                m_collection->SetItem(pair->key, pair->value);
            }

#if TEST_LOOKUP_1
            for (uint32_t i = lo; i < hi; i++)
            {
                Pair* pair = &params->pairs[i];
                uint32_t value = m_collection->GetItem(pair->key);
                if (value != pair->value)
                    return false;
            }
#endif        

            return true;
        }

        virtual bool CheckCollection(TrialParams* params)
        {
            for (uint32_t i = 0; i < params->numPairs; i++)
            {
                Pair* pair = &params->pairs[i];
                if (m_collection->GetItem(pair->key) != pair->value)
                    return false;
            }
            return true;
        }
    };

    //--------------------------------------------------------------
    //  Return an IExperiment object suitable for passing to LoopForever.
    //--------------------------------------------------------------
    IExperiment* CreateExperiment(BasePlugin* plugin, uint32_t numItems, uint32_t tableSize);
} // namespace CollectionTest1


#endif // __COMMON_COLLECTIONTEST1_H__
