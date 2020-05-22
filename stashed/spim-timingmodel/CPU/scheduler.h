#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "timingmodel.h"
#include <bits/stdc++.h>

class TimingCore;

class Scheduler : public TimingComponent
{
public:
    //std::multiset <TimingEvent *, Comp_TimingEvent> * event_vec = nullptr;
    std::deque<TimingEvent *> * event_vec = nullptr;
    TimingCore * core = nullptr;
    Comp_TimingEvent comp;

    void enq(TimingEvent *event)
    {
        //info("at cycle:%lu enqueue (%s)", event->current_cycle, typeid(*event).name());
        //event_vec->insert(event);

        if (event_vec->size() == 0) {
            event_vec->push_front(event);
            return;
        }
        TimingEvent * min_event = event_vec->front();
        if (comp(min_event, event)) { // choose event
            event_vec->push_front(event);
        } else {
            event_vec->push_back(event);
        }
    }

    void deq();

    bool is_empty()
    {
        return (event_vec->size() == 0);
    }

    Scheduler(TimingComponent * _parent);

    void Issue(TimingEvent * event) {}

    void printSched();

    ncycle_t total_inst = 0, total_cycle = 0, total_branch = 0, stall_branch = 0, total_mem = 0, stall_mem = 0, total_reg = 0, stall_reg = 0; // Only for counting
};

#endif // SCHEDULER_H