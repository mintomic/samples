#include "collectiontest1.h"
#include "experiment.h"
#include <mintomic/mintomic.h>
#include <mintsystem/timer.h>
#include <mintpack/lwlogger.h>
#include <mintpack/threadsynchronizer.h>
#include <mintpack/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


//--------------------------------------------------------------
//  In this experiment, each thread adds a different list of items to the
//  collection. No two threads add the same key.
//--------------------------------------------------------------
namespace CollectionTest1
{
    struct Result
    {
        TrialParams params;
        BasePlugin* plugin;
        mint_atomic32_t failed;
    };
    
    void threadFunc(int threadNum, void* param)
    {
        Result* result = (Result*) param;

        LWLOG("Begin loop", threadNum);
        if (!result->plugin->DoThread(threadNum, &result->params))
            mint_store_32_relaxed(&result->failed, 1);
        LWLOG("End loop", threadNum);
    }

    class Experiment : public IExperiment
    {
    private:
        BasePlugin* m_plugin;
        uint32_t m_numPairs;
        uint32_t m_tableSize;
        int m_numThreads;

    public:
        Experiment(BasePlugin* plugin, uint32_t numPairs, uint32_t tableSize, int numThreads)
            : m_plugin(plugin)
            , m_numPairs(numPairs)
            , m_tableSize(tableSize)
            , m_numThreads(numThreads)
        {
        }

        void Run(TimeKeeper& timeKeeper)
        {
            // Warm up some threads
            ThreadSynchronizer threads(m_numThreads);

            // Allocate storage
            Result result;
            result.params.pairs = new Pair[m_numPairs];
            result.params.numPairs = m_numPairs;
            result.params.numThreads = m_numThreads;
            result.plugin = m_plugin;
            result.failed._nonatomic = 0;
        
            // Fill in pairs
            Random keySeq;
            Random valueSeq;        
            for (uint32_t i = 0; i < m_numPairs; i++)
            {
                do { result.params.pairs[i].key = keySeq.generateUnique32(); } while (result.params.pairs[i].key == 0);
                do { result.params.pairs[i].value = valueSeq.generateUnique32(); } while (result.params.pairs[i].value == 0);
            }

            // Kick threads through the experiment several times
            for (int i = 0; i < 10; i++)
            {
                m_plugin->CreateCollection(m_tableSize);
                threads.run(threadFunc, &result.params);

                // Check result
                if (result.failed._nonatomic || !m_plugin->CheckCollection(&result.params))
                    timeKeeper.failures++;

                // Return average thread time
                timeKeeper.totalTime += threads.getAverageThreadRunningTime();
                timeKeeper.trials++;
            }
        }
    };

    IExperiment* CreateExperiment(BasePlugin* plugin)
    {
        return new Experiment(plugin, 4000, 4096, 2);
    }
} // namespace CollectionTest1
