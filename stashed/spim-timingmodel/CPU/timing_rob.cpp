#include "timing_rob.h"
#include "log.h"
#include "syscall.h"
#include "run.h"
#include "timing_core.h"

void TimingROB::Issue(TimingEvent * event) {
    // simulate spim at the same time, for comparison
    mem_addr old_pc = PC;
    run_spim(event->pc_addr, 1, true /* display */);

    available_cycle = MAX(event->current_cycle, available_cycle) + c_reg_write_latency;
    event->current_cycle = available_cycle;
    event->execute_cycles = event->execute_cycles + c_reg_write_latency;

    // handle reg-related inst
    if (event->inst_is_syscall) {
        // handle syscall
        reg_word v0_val;
        core->regfile->Load(0x2, v0_val);
        if (v0_val >= 1 && v0_val <= 4) {
            // printf, do nothing due to shared console
        } else if (v0_val == READ_INT_SYSCALL) {
            // read integer: copy results from R[REG_RES] to our corresponding reg
            core->regfile->Store(REG_RES, R[REG_RES]);
            //fprintf(stderr, "reg %d is unlocked!\n", REG_RES);
            --core->regfile->reg_used[REG_RES];
        }
    } else {
        if (event->RegWrite) {
            if (event->RegWrite == 2)
                event->reg_wb_data &= 0xffff;
            else if (event->RegWrite == 3)
                event->reg_wb_data &= 0xff;
            core->regfile->Store(event->reg_wb_id, event->reg_wb_data);
            //fprintf(stderr, "reg %d is unlocked!\n", event->reg_wb_id);
            --core->regfile->reg_used[event->reg_wb_id];
        }
    }

    assert(event->state == TES_WaitROB);
    event->state = TES_Committed;

    // check the consistency of PC and regfile
    assert_msg_ifnot(old_pc == event->pc_addr, "\tpc: %p should be %p", event->pc_addr, PC);
    assert_msg_ifnot(core->regfile->CheckCorrectness() == 0, "instruction %s is incorrect", inst_to_string(event->pc_addr));
}

TimingROB::TimingROB(TimingComponent * _parent)
{
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}