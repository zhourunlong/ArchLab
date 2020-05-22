#include "assert.h"
#include "timing_lsu.h"
#include "timing_core.h"

void TimingLSU::Issue(TimingEvent * event) {
    mem_word memory_read_data = 0x0;

    if (core->cache_enabled) {
        switch (event->MemRead) {
            case 0x0:
                break;
            case 0x1:
                core->cache->Access(event->alu_result, false, memory_read_data, event->pc_addr, event->current_cycle, 4);
                break;
            case 0x2:
                core->cache->Access(event->alu_result, false, memory_read_data, event->pc_addr, event->current_cycle, 4);
                memory_read_data &= 0xffff;
                break;
            case 0x3:
                core->cache->Access(event->alu_result, false, memory_read_data, event->pc_addr, event->current_cycle, 4);
                memory_read_data &= 0xff;
                break;
        }
        switch (event->MemWrite) {
            case 0x0:
                break;
            case 0x1:
                core->cache->Access(event->alu_result, true, event->mem_write_data, event->pc_addr, event->current_cycle, 4);
                break;
            case 0x2:
                core->cache->Access(event->alu_result, true, event->mem_write_data, event->pc_addr, event->current_cycle, 2);
                break;
            case 0x3:
                core->cache->Access(event->alu_result, true, event->mem_write_data, event->pc_addr, event->current_cycle, 1);
                break;
        }
    } else {
        switch (event->MemRead) {
            case 0x0:
                break;
            case 0x1:
                LOAD_INST(&memory_read_data, read_mem_word(event->alu_result), 0xffffffff);
                break;
            case 0x2:
                LOAD_INST(&memory_read_data, read_mem_half(event->alu_result), 0xffff);
                break;
            case 0x3:
                LOAD_INST(&memory_read_data, read_mem_byte(event->alu_result), 0xff);
                break;
        }
        switch (event->MemWrite) {
            case 0x0:
                break;
            case 0x1:
                set_mem_word(event->alu_result, event->mem_write_data);
                break;
            case 0x2:
                set_mem_half(event->alu_result, event->mem_write_data & 0xffff);
                break;
            case 0x3:
                set_mem_byte(event->alu_result, event->mem_write_data & 0xff);
                break;
        }
    }
    //fprintf(stderr, "addr = %08x read = %08x written = %08x\n", event->alu_result, memory_read_data, event->mem_write_data);
    // assert_msg_ifnot(event->MemToReg != -1, "instruction %s has not been fully implemented", inst_to_string(event->pc_addr));
    if (event->MemToReg == 0x0)
        event->reg_wb_data = event->alu_result;
    else if (event->MemToReg == 0x1)
        event->reg_wb_data = memory_read_data;

    if (event->MemRead || event->MemWrite) {
        if (core->cache_enabled) {
            available_cycle = MAX(event->current_cycle, available_cycle);  // available_cycle is updated by timing cache
        } else {
            available_cycle = MAX(event->current_cycle, available_cycle) + c_memory_access_latency;
            event->current_cycle = available_cycle;
            event->execute_cycles += c_memory_access_latency;
        }
    } else {
        // this stage still count 1 cycle even when no memory request
        available_cycle = MAX(event->current_cycle, available_cycle) + 1;
        event->current_cycle = available_cycle;
        event->execute_cycles += 1;
    }

    assert(event->state == TES_WaitLSU);
    event->state = TES_WaitROB;
}

TimingLSU::TimingLSU(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}