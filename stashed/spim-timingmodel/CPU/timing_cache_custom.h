
#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "timing_cache.h"
#include "lab3_def.h"

namespace MemoryHierarchy {
class TimingCache;

// address translation:
// |----tag 23b ----|---idx 4b---|-offset5b-|
const int c_asso_num = 2;
struct CacheTagEntry {
    char valid;
    char dirty;
    char fifo_ptr = 0;
    short data[c_asso_num];
    mem_addr tag[c_asso_num];
};

// define more constants
const int c_cache_set_num = c_cache_block_num / c_asso_num;  // 16
const int c_addr_idx_bit = log(c_cache_set_num) / log(2);
const int c_addr_idx_mask = ((1 << c_addr_idx_bit) - 1) << c_addr_offset_bit;
const int c_buffer_idx = c_addr_idx_mask >> c_addr_offset_bit;
const int c_tag_entry_size = sizeof(CacheTagEntry);  // 8B for each cache line
const int c_tag_array_size = c_tag_entry_size * c_cache_set_num;  // 8B * 2 * 16 = 256B < capacity

const bool DEBUG = false;
const mem_addr target_debug_addr = 0x10010420;
const mem_addr target_debug_tag = (unsigned int)target_debug_addr >> c_addr_offset_bit;
const mem_addr target_debug_offset = 0x40;

/* switching replacement policies */
enum Policies {
    FIFO, RANDOM, LFU, LRU_MIP, LRU_LIP
};
const Policies policy = FIFO;

/* controlling whether to use off-critical path, true = use */
const bool OffCriticalPath = false;

/* setting the maximum size of buffer, <= c_asso_num*/
const int buffer_size = 2;

/* using direct map */
const bool DirectMap = false;

class CacheController {
private:
    MemoryHierarchy::TimingCache *cache;
    void AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset);
    void MergeBlock(char* blk, reg_word wdata, int offset, int store_data_size);
    bool EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset);
    bool UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset);
public:
    void Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, int store_data_size);
    void Reset();
    // Process off the critical operation. Now you can solve somethind not urgent, e.g., writing cacheline back to memory. This still increments available_cycle but may not influence current instruction.
    void ProcessOffCritical(mem_addr addr, bool is_write, mem_addr PC);
    void DisplayStats();
    CacheController(MemoryHierarchy::TimingCache *cache_);
    // do not change the above methods. TimingCache will call with their exact names.

    // define your own functions
    void ResetTagArray();
    void PushIntoBuffer(mem_addr addr, char *data);
    void FindInBuffer(mem_addr addr, bool &is_hit, cache_line &data, int &hit_offset);
    int CalcIndexFromAddress(mem_addr addr);
    mem_addr CalcAddressFromIndex(mem_addr tag, int idx);
};

}

#endif // CACHE_CONTROLLER_H