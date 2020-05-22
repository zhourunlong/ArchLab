#ifndef LAB3_H
#define LAB3_H
#include <cmath>

namespace MemoryHierarchy {

/* basic definitions */
const int c_cache_line_size = 32;  // cache line size is 32B
const int c_addr_offset_bit = log(c_cache_line_size) / log(2);  // log2(c_cache_line_size)
const int c_addr_cache_line_mask = (1 << c_addr_offset_bit) - 1;  // get the least 5 bits
const int c_cache_capacity = 1 * K;  // capacity (size of data array) is 1KB
typedef char cache_line[c_cache_line_size];
const int c_cache_block_num = c_cache_capacity / c_cache_line_size;  // 64 cachelines
const int c_tag_arr_capacity =  0.5 * K;  // capacity (size of tag array) is 512B
const int c_tag_entry_size_maximum = 32;  // maximum tag entry size is 32B

}

#endif // LAB3_H