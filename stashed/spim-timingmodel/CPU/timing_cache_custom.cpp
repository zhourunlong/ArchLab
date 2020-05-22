#include "timing_cache_custom.h"
namespace MemoryHierarchy {

// return true and offset is the id of the victim way. return false if refusing eviction.
bool CacheController::EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset) {
    int idx = CalcIndexFromAddress(addr);
    if (OffCriticalPath && idx == c_buffer_idx)
        return false;
    mem_addr tag = (unsigned int)addr >> (c_addr_offset_bit + c_addr_idx_bit);
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    int mi = 999999999, i;
    bool find_empty = false;
    for (int j = 0; j < c_asso_num; ++j) {
        if (!((entry->valid >> j) & 1)) {
            i = j;
            find_empty = true;
            break;
        }
    }
    if (!find_empty) {
        switch (policy) {
            case FIFO:
                entry->fifo_ptr = (entry->fifo_ptr + 1) % c_asso_num;
                i = entry->fifo_ptr;
                break;
            case RANDOM:
                i = rand() % c_asso_num;
                break;
            default:
                for (int j = 0; j < c_asso_num; ++j)
                    if (entry->data[j] < mi) {
                        mi = entry->data[j];
                        i = j;
                    }
                break;
        }
    }
    offset = (idx * c_asso_num + i) * c_cache_line_size;
    if (((entry->valid >> i) & 1) && ((entry->dirty >> i) & 1)) {
        char data_buf[c_cache_line_size];
        cache->data_array->Get(offset, data_buf);

        mem_addr wr_addr = CalcAddressFromIndex(entry->tag[i], idx);
if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || (wr_addr >> c_addr_offset_bit) == target_debug_tag)) {
    fprintf(stderr, "evicted: way=%x, idx=%x, tag=%x, (tag,idx)=%x, offset=%x\n", i, idx, entry->tag[i], wr_addr, offset);
}
        if (OffCriticalPath)
            PushIntoBuffer(wr_addr, data_buf);
        else
            cache->WriteMemoryData(wr_addr, data_buf);
    }
    int clk = 0;
    for (int j = 0; j < c_asso_num; ++j)
        if (((entry->valid >> j) & 1) && entry->data[j] > clk)
            clk = entry->data[j];
    switch (policy) {
        case LFU:
            entry->data[i] = 1;
            break;
        case LRU_MIP:
            entry->data[i] = clk + 1;
            int fix = 999999999;
            for (int j = 0; j < c_asso_num; ++j)
                if (((entry->valid >> j) & 1) && entry->data[j] < fix)
                    fix = entry->data[j];
            for (int j = 0; j < c_asso_num; ++j)
                entry->data[j] -= fix;
            break;
    }
    entry->valid &= ((1 << c_asso_num) - 1) ^ (1 << i);
    entry->dirty &= ((1 << c_asso_num) - 1) ^ (1 << i);
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    return true;
}

// called this to update tag entry after allocating a new cache block, or a hit
bool CacheController::UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset) {
    int idx = offset / c_cache_line_size / c_asso_num;   // hit_offset = (idx * c_asso_num + way) * c_cache_line_size;  SOMETIMES IT HITS IN THE BUFFER, SO CANNOT CALCULATE IDX ACCORDING TO ADDRESS!!!
    mem_addr tag = (unsigned int)addr >> (c_addr_offset_bit + c_addr_idx_bit);
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    int way = (offset / c_cache_line_size) % c_asso_num; // hit_offset = (idx * c_asso_num + way) * c_cache_line_size;
    entry->valid |= 1 << way;
    if (is_write)
        entry->dirty |= 1 << way;
    else
        entry->dirty &= ((1 << c_asso_num) - 1) ^ (1 << way);
    if (OffCriticalPath && idx == c_buffer_idx)
        entry->tag[way] = (unsigned int)addr >> c_addr_offset_bit;
    else
        entry->tag[way] = tag;
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    return false;
}

// check if the addr is a hit. data is set to hit cacheline and hit_offset is its data array pointer.
void CacheController::AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset) {
    int idx = CalcIndexFromAddress(addr);
    mem_addr tag = (unsigned int)addr >> (c_addr_offset_bit + c_addr_idx_bit);
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    is_hit = false;
    int i;
    int max_ways = c_asso_num;
    if (OffCriticalPath && idx == c_buffer_idx) {
        tag = (unsigned int)addr >> c_addr_offset_bit;
        max_ways = entry->fifo_ptr;
    }
    for (i = 0; i < max_ways; ++i)
        if (((entry->valid >> i) & 1) && entry->tag[i] == tag) {
            is_hit = true;
            break;
        }
    if (is_hit) {
        int clk = 0;
        for (int j = 0; j < c_asso_num; ++j)
            if (((entry->valid >> j) & 1) && entry->data[j] > clk)
                clk = entry->data[j];
        switch (policy) {
            case LFU:
                ++entry->data[i];
                break;
            default:
                entry->data[i] = clk + 1;
                int fix = 999999999;
                for (int j = 0; j < c_asso_num; ++j)
                    if (((entry->valid >> j) & 1) && entry->data[j] < fix)
                        fix = entry->data[j];
                for (int j = 0; j < c_asso_num; ++j)
                    entry->data[j] -= fix;
                break;
        }
        cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
        hit_offset = (idx * c_asso_num + i) * c_cache_line_size;
        cache->data_array->Get(hit_offset, data);
    } else {
        if (OffCriticalPath && idx != c_buffer_idx)
            FindInBuffer(addr, is_hit, data, hit_offset);

if (is_hit && DEBUG && OffCriticalPath && (target_debug_addr == 0 || tag == target_debug_tag))
    fprintf(stderr, "addr: %lx hit in buffer\n", addr);

    }
}

// Only need to set Valid to 0
void CacheController::ResetTagArray() {
    CacheTagEntry *entry = nullptr;
    char buf[c_tag_entry_size];
    for (int idx = 0; idx < c_cache_set_num; ++idx) {
        cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
        entry = (CacheTagEntry *)buf;
        entry->fifo_ptr = 0;
        entry->valid = 0;
        entry->dirty = 0;
        memset(entry->data, 0, sizeof(entry->data));
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

int ftag = (unsigned int)addr >> c_addr_offset_bit;
if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag))
    fprintf(stderr, "ack a request addr:%lx is_write:%x wrdata:%x\n", addr, is_write, wrdata);

    int offset = addr & c_addr_cache_line_mask;
    cache_line data_blk;
    bool is_hit;
    int hit_offset;  // the offset of the hit cacheline
    AccessTagArray(addr, PC, is_hit, data_blk, hit_offset);
    if (is_hit) {

if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag))
    fprintf(stderr, "hit, dataptr:%x\n", hit_offset);

        if (!is_write)
            memcpy(&wrdata, (char *)(&data_blk) + offset, sizeof(reg_word));
        else {
            MergeBlock(data_blk, wrdata, offset, store_data_size);

if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag)) {
    fprintf(stderr, "data_blk=\n  ");
    for (int x = 0; x < c_cache_line_size; ++x)
        fprintf(stderr, "%02x,", (unsigned char)data_blk[x]);
    fprintf(stderr, "\n");
}

            UpdateMetadata(addr, is_write, PC, hit_offset);
            cache->data_array->Set(hit_offset, data_blk);
        }
    } else {
        int evict_line_offset = -1;
        bool eviction_accepted = EvictCacheline(addr, is_write, PC, evict_line_offset);
        cache->FetchMemoryData(addr, (char *)&data_blk);

if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag)) {
    fprintf(stderr, "miss, evict ptr:%x\n", evict_line_offset);
    fprintf(stderr, "fetch remote data\n");
}

        if (!is_write) {
            memcpy(&wrdata, (char *)(&data_blk) + offset, sizeof(reg_word));

if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag))
    fprintf(stderr, "wrdata: %x\n", wrdata);

        } else {
            MergeBlock(data_blk, wrdata, offset, store_data_size);

if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag)) {
    fprintf(stderr, "data_blk=\n  ");
    for (int x = 0; x < c_cache_line_size; ++x)
        fprintf(stderr, "%02x,", (unsigned char)data_blk[x]);
    fprintf(stderr, "\n");
}

        }
        if (eviction_accepted) {
            assert(evict_line_offset >= 0 && evict_line_offset <= (c_cache_capacity - c_cache_line_size));
            UpdateMetadata(addr, is_write, PC, evict_line_offset);
            cache->data_array->Set(evict_line_offset, data_blk);
            
if (DEBUG && OffCriticalPath && (target_debug_addr == 0 || ftag == target_debug_tag))
    fprintf(stderr, "write to victim write data arr\n");

        } else {
            if (is_write) {
                if (OffCriticalPath)
                    PushIntoBuffer(addr, (char *)&data_blk);
                else
                    cache->WriteMemoryData(addr, (char *)&data_blk);
            }
        }
    }
}

// Display your own stats
void CacheController::DisplayStats() {}

void CacheController::FindInBuffer(mem_addr addr, bool &is_hit, cache_line &data, int &hit_offset) {
    if (!OffCriticalPath) return;

    mem_addr tag = (unsigned int)addr >> c_addr_offset_bit;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(c_buffer_idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    for (int i = 0; i < entry->fifo_ptr; ++i)
        if (((entry->valid >> i) & 1) && entry->tag[i] == tag) {
            hit_offset = (c_buffer_idx * c_asso_num + i) * c_cache_line_size;
            cache->data_array->Get(hit_offset, data);
            is_hit = true;
            return;
        }
    is_hit = false;
}

void CacheController::PushIntoBuffer(mem_addr addr, char *data) {
    if (!OffCriticalPath) return;

    mem_addr tag = (unsigned int)addr >> c_addr_offset_bit;

if (DEBUG && (target_debug_addr == 0 || tag == target_debug_tag)) {
    fprintf(stderr, "addr: %x pushed into buffer, data=\n  ", addr);
    for (int x = 0; x < c_cache_line_size; ++x)
        fprintf(stderr, "%02x,", (unsigned char)data[x]);
    fprintf(stderr, "\n");
}

    char buf[c_tag_entry_size];
    cache->tag_array->Get(c_buffer_idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    if (entry->fifo_ptr == c_asso_num) {
        cache->WriteMemoryData(addr, data);
    } else {
        entry->tag[entry->fifo_ptr] = tag;
        entry->valid |= 1 << entry->fifo_ptr;
        int offset = (c_buffer_idx * c_asso_num + entry->fifo_ptr) * c_cache_line_size;
        cache->data_array->Set(offset, data);
        ++entry->fifo_ptr;
        cache->tag_array->Set(c_buffer_idx * sizeof(CacheTagEntry), buf);
    }
}

int CacheController::CalcIndexFromAddress(mem_addr addr) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    if (DirectMap)
        return idx;
    int lastbit = (addr >> (c_addr_idx_bit + c_addr_offset_bit)) & 1;
    return idx ^ lastbit;
}

mem_addr CacheController::CalcAddressFromIndex(mem_addr tag, int idx) {
    if (!DirectMap)
        idx ^= (tag & 1);
    return (tag << (c_addr_offset_bit + c_addr_idx_bit)) | (idx << c_addr_offset_bit);
}

void CacheController::ProcessOffCritical(mem_addr addr, bool is_write, mem_addr PC) {
    if (!OffCriticalPath) return;

    char buf[c_tag_entry_size];
    cache->tag_array->Get(c_buffer_idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    if (entry->fifo_ptr >= buffer_size) {
        for (int i = 0; i < entry->fifo_ptr; ++i) {
            char data[c_cache_line_size];
            int offset = (c_buffer_idx * c_asso_num + i) * c_cache_line_size;
            cache->data_array->Get(offset, data);
            cache->WriteMemoryData(entry->tag[i] << c_addr_offset_bit, data);
        }
        entry->valid = 0;
        entry->fifo_ptr = 0;
        cache->tag_array->Set(c_buffer_idx * sizeof(CacheTagEntry), buf);
    }
}

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
