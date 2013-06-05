#include <common/arrayofitems.h>
#include <common/collectiontest1.h>
#include <common/collectiontest2.h>
#include <common/experiment.h>
#include <mintsystem/timer.h>
#include <vector>


int main()
{
    mint_timer_initialize();

    std::vector<IExperiment*> exps;
    CollectionTest1::Plugin<ArrayOfItems> handler;
    exps.push_back(CollectionTest1::CreateExperiment(&handler));
    CollectionTest2::Plugin<ArrayOfItems> handler2;
    exps.push_back(CollectionTest2::CreateExperiment(&handler2));

    LoopForever(exps);

    for (size_t i = 0; i < exps.size(); i++)
        delete exps[i];
    return 0;
}
