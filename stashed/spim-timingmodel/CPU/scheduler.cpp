#include "scheduler.h"
#include "timing_core.h"
#include "assert.h"

/*
void Scheduler::printSched() {
    printf("size:%lu ", event_vec->size());
    for (auto p : *event_vec)
        printf(", 0x%x %x %llu", p->pc_addr, p->state, p->current_cycle);
    printf("\n");
}
*/

void Scheduler::printSched() {
    printf("size:%lu ", event_vec->size());
    for (unsigned int i = 0; i < event_vec->size(); i++) {
        printf(", 0x%x %x", event_vec->at(i)->pc_addr, event_vec->at(i)->state);
    }
    printf("\n");
}

void Scheduler::deq() {
    if (is_empty())
        return;

    /*
    TimingEvent *event = *(event_vec->begin());
    event_vec->erase(event_vec->begin());
    */

    TimingEvent *event = event_vec->front();
    event_vec->pop_front();

    if (event_vec->size() > 0) {
        auto min_event_id = event_vec->begin();
        for (auto event_id = event_vec->begin(); event_id != event_vec->end(); event_id++) {
            if (comp(*min_event_id, *event_id)) {
                min_event_id = event_id;
            }
        }
        TimingEvent * tmp = event_vec->front();
        event_vec->front() = *min_event_id;
        *min_event_id = tmp;
    }

    //fprintf(stderr, "%lu: addr: %p state:%x scycle:%d\n", event->current_cycle, event->pc_addr, event->state, event->start_cycle);
    // printSched();

    switch (event->state) {
        case TES_WaitFetcher:
            core->fetcher->Issue(event);
            enq(event);
            break;
        case TES_WaitDecoder:
            core->decoder->Issue(event);
            enq(event);
            break;
        case TES_WaitExecutor:
            core->executor->Issue(event);
            enq(event);
            break;
        case TES_WaitLSU:
            core->lsu->Issue(event);
            enq(event);
            break;
        case TES_WaitROB:
            core->rob->Issue(event);
            enq(event);
            break;
        case TES_Committed:
            core->s_total_inst++;
            //fprintf(stderr,"inst pc:%x cycle:%d\n", event->pc_addr, event->current_cycle);
            core->s_total_cycle = MAX(core->s_total_cycle, event->current_cycle);
            ++total_inst;
            total_cycle = std::max(total_cycle, event->current_cycle);
            if (event->Branch) {
                ++total_branch;
                stall_branch += event->stall;
            } else if (event->Mem) {
                ++total_mem;
                stall_mem += event->stall;
            } else if (event->Reg) {
                ++total_reg;
                stall_reg += event->stall;
            }
            delete event;
            break;
        default:
            printf("Unknown event type!\n");
            assert(false);
    }
}

Scheduler::Scheduler(TimingComponent * _parent)
{
    //event_vec = new std::multiset <TimingEvent *, Comp_TimingEvent>;
    event_vec = new std::deque<TimingEvent *>;
    core = dynamic_cast<TimingCore *>(_parent);
}