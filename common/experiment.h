#ifndef __COMMON_EXPERIMENT_H__
#define __COMMON_EXPERIMENT_H__

#include <mintomic/core.h>
#include <mintsystem/timer.h>
#include <vector>


//--------------------------------------------------------------
//  Maintain a total of running times, trials and failures
//--------------------------------------------------------------
struct TimeKeeper
{
    mint_timer_tick_t totalTime;
    uint32_t trials;
    uint32_t failures;

    TimeKeeper()
    {
        totalTime = 0;
        trials = 0;
        failures = 0;
    }
    double getAverageTime() const
    {
        if (trials == 0)
            return 0;
        return totalTime * mint_timer_ticksToSeconds / trials;
    }
};
        
//--------------------------------------------------------------
//  Base interface for any experiment that runs ad infinitum
//--------------------------------------------------------------
class IExperiment
{
public:
    virtual ~IExperiment() {};
    virtual void Run(TimeKeeper& timeKeeper) = 0;
};

//--------------------------------------------------------------
//  Run a set of experiments indefinitely while logging stats
//--------------------------------------------------------------
void LoopForever(const std::vector<IExperiment*>& exps);


#endif // __COMMON_EXPERIMENT_H__
