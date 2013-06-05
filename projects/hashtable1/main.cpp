#include <common/hashtable1.h>
#include <common/collectiontest1.h>
#include <common/collectiontest2.h>
#include <common/experiment.h>
#include <mintsystem/timer.h>
#include <vector>


int main()
{
    mint_timer_initialize();

    std::vector<IExperiment*> exps;
    CollectionTest1::Plugin<HashTable1> handler;
    exps.push_back(CollectionTest1::CreateExperiment(&handler, 12000, 16384));
    CollectionTest2::Plugin<HashTable1> handler2;
    exps.push_back(CollectionTest2::CreateExperiment(&handler2, 12000, 16384));

    LoopForever(exps);

    for (size_t i = 0; i < exps.size(); i++)
        delete exps[i];
    return 0;
}
