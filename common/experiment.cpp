#include "experiment.h"
#include <mintsystem/timer.h>
#include <stdio.h>
#include <stdlib.h>


//--------------------------------------------------------------
//  Run a set of experiments indefinitely while logging stats
//--------------------------------------------------------------
void LoopForever(const std::vector<IExperiment*>& exps)
{
    TimeKeeper* timeKeepers = new TimeKeeper[exps.size()];

    mint_timer_tick_t logInterval = (mint_timer_tick_t) (1 * mint_timer_secondsToTicks);
    mint_timer_tick_t nextLog = mint_timer_get() + logInterval;
    for (;;)
    {
        for (size_t i = 0; i < exps.size(); i++)
        {
            exps[i]->Run(timeKeepers[i]);
        }

        mint_timer_tick_t now = mint_timer_get();
        if (mint_timer_greater_or_equal(now, nextLog))
        {
            printf("------------------------------------\n");
            for (size_t j = 0; j < exps.size(); j++)
                printf("experiment #%d: %d trials, %d failures, average=%f ms\n", (int) j, timeKeepers[j].trials, timeKeepers[j].failures, timeKeepers[j].getAverageTime() * 1000);
            nextLog = now + logInterval;
        }
    }

    delete[] timeKeepers;
}
