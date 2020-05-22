#include "timing_cache_custom.h"
namespace MemoryHierarchy {

// return true and offset is the id of the victim way. return false if refusing eviction.
bool CacheController::EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    int tag = (unsigned int)addr >> 9;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    int mi = 999999999, i;
    for (int j = 0; j < c_asso_num; ++j) {
        if (!entry->valid[j]) {
            i = j;
            break;
        }
        if (entry->time_stamp[j] < mi) {
            mi = entry->time_stamp[j];
            i = j;
        }
    }
    offset = (idx * c_asso_num + i) * c_cache_line_size;
    if (entry->valid[i] && entry->dirty[i]) {
        char data_buf[c_cache_line_size];
        cache->data_array->Get(offset, data_buf);
        cache->WriteMemoryData((entry->tag[i] << 9) | (idx << c_addr_offset_bit), data_buf);
    }
    entry->time_stamp[i] = entry->clk++;
    entry->valid[i] = false;
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    return true;
}

// called this to update tag entry after allocating a new cache block, or a hit
bool CacheController::UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    int tag = (unsigned int)addr >> 9;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    int way = (offset / c_cache_line_size) % c_asso_num; // hit_offset = (idx * c_asso_num + way) * c_cache_line_size;
    entry->valid[way] = true;
    entry->dirty[way] = is_write;
    entry->tag[way] = tag;
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    return false;
}

// check if the addr is a hit. data is set to hit cacheline and hit_offset is its data array pointer.
void CacheController::AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    int tag = (unsigned int)addr >> 9;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    is_hit = false;
    int i;
    for (i = 0; i < c_asso_num; ++i)
        if (entry->valid[i] && entry->tag[i] == tag) {
            is_hit = true;
            break;
        }
    if (is_hit) {
        entry->time_stamp[i] = entry->clk++;
        hit_offset = (idx * c_asso_num + i) * c_cache_line_size;
        cache->data_array->Get(hit_offset, data);
    }
}

// Only need to set Valid to 0
void CacheController::ResetTagArray() {
    CacheTagEntry *entry = nullptr;
    char buf[c_tag_entry_size];
    for (int idx = 0; idx < c_cache_set_num; ++idx) {
        cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
        entry = (CacheTagEntry *)buf;
        entry->clk = 0;
        for (int way = 0; way < c_asso_num; ++way) {
            entry->valid[way] = false;
            entry->freq[way] = 0;
        }
        cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    }
}

void CacheController::Reset() {
    ResetTagArray();
}

// Merge wdata (one word) into a cacheline
void CacheController::MergeBlock(char* blk, reg_word wdata, int offset, int store_data_size) {
    memcpy(blk + offset, &wdata, store_data_size);
}

// Handling requests from TimingCache
void CacheController::Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, int store_data_size) {
    //fprintf(stderr, "ack a request addr:%lx is_write:%x wrdata:%x\n", addr, is_write, wrdata);
    int offset = addr & c_addr_cache_line_mask;
    cache_line data_blk;
    bool is_hit;
    int hit_offset;  // the offset of the hit cacheline
    AccessTagArray(addr, PC, is_hit, data_blk, hit_offset);
    if (is_hit) {
        //fprintf(stderr, "hit, dataptr:%x\n", hit_offset);
        if (!is_write)
            memcpy(&wrdata, (char *)(&data_blk) + offset, sizeof(reg_word));
        else {
            for (int x = 0; x < store_data_size; ++x)
                data_blk[offset + x] = ((unsigned)wrdata >> (x << 3)) & (0xff);
            UpdateMetadata(addr, is_write, PC, hit_offset);
            cache->data_array->Set(hit_offset, data_blk);
        }
    } else {
        int evict_line_offset = -1;
        bool eviction_accepted = EvictCacheline(addr, is_write, PC, evict_line_offset);
        //fprintf(stderr, "miss, evict ptr:%x\n", evict_line_offset);
        //fprintf(stderr, "fetch remote data\n");
        cache->FetchMemoryData(addr, (char *)&data_blk);
        if (!is_write)
            memcpy(&wrdata, (char *)(&data_blk) + offset, sizeof(reg_word));
        else {
            for (int x = 0; x < store_data_size; ++x)
                data_blk[offset + x] = ((unsigned)wrdata >> (x << 3)) & (0xff);
        }
        if (eviction_accepted) {
            assert(evict_line_offset >= 0 && evict_line_offset <= (c_cache_capacity - c_cache_line_size));
            UpdateMetadata(addr, is_write, PC, evict_line_offset);
            cache->data_array->Set(evict_line_offset, data_blk);
            //fprintf(stderr, "write to victim write data arr\n");
        } else {
            if (is_write)
                cache->WriteMemoryData(addr, (char *)&data_blk);
        }
    }
}

// Display your own stats
void CacheController::DisplayStats() {}

void CacheController::ProcessOffCritical(mem_addr addr, bool is_write, mem_addr PC) {}

// Initilization. Set a default block access granularity. Reset tag array.
CacheController::CacheController(MemoryHierarchy::TimingCache *cache_) {
    cache = cache_;
    if (cache->data_array)
        cache->data_array->SetBlockSize(c_cache_line_size);
    if (cache->tag_array)
        cache->tag_array->SetBlockSize(c_tag_entry_size);
    ResetTagArray();
}

}
