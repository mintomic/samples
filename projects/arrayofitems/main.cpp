#include <common/arrayofitems.h>
#include <mintpack/threadsynchronizer.h>
#include <mintpack/lwlogger.h>
#include <mintpack/random.h>
#include <mintsystem/timer.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_LOOKUP 1

struct Pair
{
    uint32_t key;
    uint32_t value;
};

class TestBed
{
private:
    static TestBed* s_instance;
    
    Pair* m_pairs;
    uint32_t m_numPairs;
    int m_numThreads;
    ArrayOfItems* m_collection;
#if TEST_LOOKUP
    mint_atomic32_t m_failed;
#endif    
    
    void doThread(int arg)
    {
        LWLOG("Begin loop", arg);
        
        uint32_t lo = m_numPairs * arg / m_numThreads;
        uint32_t hi = m_numPairs * (arg + 1) / m_numThreads;
        
        for (uint32_t i = lo; i < hi; i++)
        {
            Pair* pair = &m_pairs[i];
            m_collection->SetItem(pair->key, pair->value);
        }

#if TEST_LOOKUP
        for (uint32_t i = lo; i < hi; i++)
        {
            Pair* pair = &m_pairs[i];
            uint32_t value = m_collection->GetItem(pair->key);
            if (value != pair->value)
                mint_store_32_relaxed(&m_failed, 1);
        }
#endif        

        LWLOG("End loop", arg);
    }

    static void threadFunc(int arg)
    {
        s_instance->doThread(arg);
    }
        
public:
    TestBed()
        : m_pairs(NULL)
        , m_numPairs(0)
        , m_numThreads(0)
        , m_collection(NULL)
    {
        assert(s_instance == NULL);
        s_instance = this;

#if TEST_LOOKUP
        m_failed._nonatomic = 0;
#endif
    }

    ~TestBed()
    {
        delete[] m_pairs;
        delete m_collection;
        s_instance = NULL;
    }

    mint_timer_tick_t test(uint32_t numPairs, uint32_t tableSize, int numThreads)
    {
        assert(!m_pairs);
        assert(!m_collection);

        // Allocate storage
        m_pairs = new Pair[numPairs];
        m_numPairs = numPairs;
        m_numThreads = numThreads;
        m_collection = new ArrayOfItems(tableSize);
        
        // Fill in pairs
        Random keySeq;
        Random valueSeq;        
        for (uint32_t i = 0; i < numPairs; i++)
        {
            do { m_pairs[i].key = keySeq.generateUnique(); } while (m_pairs[i].key == 0);
            do { m_pairs[i].value = valueSeq.generateUnique(); } while (m_pairs[i].value == 0);
        }

        // Kick off threads
        ThreadSynchronizer threads(numThreads);
        mint_timer_tick_t start = mint_timer_get();
        threads.run(threadFunc);

        // Check result
#if TEST_LOOKUP
        if (m_failed._nonatomic)
            exit(1);
#endif
        for (uint32_t i = 0; i < numPairs; i++)
        {
            Pair* pair = &m_pairs[i];
            if (m_collection->GetItem(pair->key) != pair->value)
                exit(1);
        }

        // Return average thread time
        mint_timer_tick_t total = 0;
        for (int t = 0; t < numThreads; t++)
            total += threads.getThreadRunningTime(t);
        return total / numThreads;
    }
};

TestBed* TestBed::s_instance = NULL;

int main()
{
    mint_timer_initialize();

    mint_timer_tick_t logInterval = (mint_timer_tick_t) (1 * mint_timer_secondsToTicks);
    mint_timer_tick_t nextLog = mint_timer_get() + logInterval;
    mint_timer_tick_t totalTime = 0;
    int trial;
    for (trial = 1; trial < 10000000; trial++)
    {
        TestBed testBed;
        totalTime += testBed.test(4000, 4096, 2);
        mint_timer_tick_t now = mint_timer_get();
        if (mint_timer_greater_or_equal(now, nextLog))
        {
            printf("%d iterations, average = %f millisecs.\n", trial, totalTime * mint_timer_ticksToSeconds * 1000 / trial);
            nextLog = now + logInterval;
        }
    }
    return 0;
}
