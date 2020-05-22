#include "assert.h"
#include "timing_executor.h"
#include "timing_core.h"

void TimingExecutor::Issue(TimingEvent * event) {
    event->alu_result = core->alu->Execute(event->alu_src_1, event->alu_src_2, event->ALUOp);
    
    /*
    if (event->delay_branch) {
        //printf("ALU_result = %d\n", event->alu_result);
        event->stall += MAX(event->current_cycle, available_cycle) + c_executor_latency - event->current_cycle;
    }
    */

    available_cycle = MAX(event->current_cycle, available_cycle) + c_executor_latency;
    event->current_cycle = available_cycle;
    event->execute_cycles += c_executor_latency;
    
    //if (event->delay_branch)
    //    core->next_pc_gen->GenerateNextPC(event->pc_addr, event->extended_imm, 0, event->alu_result, 0, event->current_cycle, false);

    assert(event->state == TES_WaitExecutor);
    event->state = TES_WaitLSU;
}

TimingExecutor::TimingExecutor(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}