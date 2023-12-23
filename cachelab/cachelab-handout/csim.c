#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

int hits=0, misses=0, evictions=0;

uint64_t running_time=0; // global time, ++ after each parser

uint32_t set_index_bits; // s
uint32_t lines_per_set; // E
uint32_t block_bits; // b
uint32_t addr_len = 64;
char* trace_file; // t

#define GET_BIT_RANGE_64(num, low, high) \
            ((num >> low) & ((1ULL << (high - low + 1)) - 1))

typedef struct 
{
    /* data */
    // uint32_t set;
    uint64_t tag;
    uint64_t last_visit_time;
    bool valid;
}cacheLine;

cacheLine* cache_ptr;

cacheLine* findLRU(cacheLine* cache_line_begin){
    // find maxium of running_time-last_visit_time
    cacheLine* cache_line_find = cache_line_begin;
    for(int i=0; i<lines_per_set; ++i){
        if(!cache_line_begin[i].valid){
            cache_line_find = cache_line_begin+i;
            return cache_line_find;
        }
    }
    ++evictions;
    printf(" EVICTION ");
    uint64_t lru_time = running_time - cache_line_begin[0].last_visit_time;
    for(int i=0; i<lines_per_set; ++i){
        uint64_t temp_lru_time = running_time - cache_line_begin[i].last_visit_time;
        if(temp_lru_time>lru_time){
            lru_time = temp_lru_time;
            cache_line_find = cache_line_begin+i;
        }
    }
    return cache_line_find;
}

void load(uint64_t tag, uint64_t set_index, \
            uint64_t block_offset, int size){
    // find the set begin: cache_ptr[set_index*line_per_set]
    int set_index_begin = set_index*lines_per_set;
    for(int i=0; i<lines_per_set; ++i){
        if(cache_ptr[set_index_begin+i].tag==tag \
            && cache_ptr[set_index_begin+i].valid){
            // hit
            ++hits;
            printf(" HIT ");
            cache_ptr[set_index_begin+i].last_visit_time = running_time;
            return;
        }
    }
    // cache miss 
    ++misses;
    printf(" MISS ");
    // if exist line.valid=false, just fill it 
    // else LRU eviction++
    cacheLine* cache_find = findLRU(cache_ptr+set_index_begin);
    if(cache_find){
        cache_find->valid = true;
        cache_find->last_visit_time = running_time;
        cache_find->tag = tag;
    }
}

void store(uint64_t tag, uint64_t set_index, \
            uint64_t block_offset, int size){
    // store: CPU -> cache, 同样涉及到LRU策略
    // write hit
    int set_index_begin = set_index*lines_per_set;
    for(int i=0; i<lines_per_set; ++i){
        if(cache_ptr[set_index_begin+i].tag==tag \
            && cache_ptr[set_index_begin+i].valid){
            // hit
            ++hits;
            printf(" HIT ");
            cache_ptr[set_index_begin+i].last_visit_time = running_time;
            return;
        }
    }
    ++misses;
    printf(" MISS ");
    cacheLine* cache_find = findLRU(cache_ptr+set_index_begin);
    if(cache_find){
        cache_find->valid = true;
        cache_find->last_visit_time = running_time;
        cache_find->tag = tag;
    }
}

void modify(uint64_t tag, uint64_t set_index, \
            uint64_t block_offset, int size){
    load(tag, set_index, block_offset, size);
    store(tag, set_index, block_offset, size);
}

void addressParser(uint64_t address, uint64_t* block_offset, \
                    uint64_t* set_index, uint64_t* tag){
// use s, E, b to parse an address
    // uint32_t address = 0;
    // sscanf(addr, "%d", &address);
    *block_offset = GET_BIT_RANGE_64(address, 0, block_bits-1);
    *set_index = GET_BIT_RANGE_64(address, block_bits, block_bits+set_index_bits-1);
    *tag = GET_BIT_RANGE_64(address, (block_bits+set_index_bits), addr_len-1);
    // printf("%lx, %lx, %lx\n", *tag, *set_index, *block_offset);
}

int traceParser(char* trace_file){
    FILE* trace_pointer = fopen(trace_file, "r");
    if(!trace_pointer){
        printf("Fail to load file\n");
        return 1;
    }
    char trace_line[255];
    while (fgets(trace_line, 255, trace_pointer) != NULL)
    {
        /* code */
        if(*trace_line == ' '){
            printf("%s", trace_line);
            char* endptr;
            int size;
            uint64_t block_offset, set_index, tag;
            uint64_t address = strtol(trace_line+2, &endptr, 16);
            sscanf(endptr+1, "%d", &size);
            addressParser(address, &block_offset, &set_index, &tag);
            switch (*(trace_line+1)){
            case 'L':
                /* code */
                load(tag, set_index, block_offset, size);
                break;
            case 'M':
                modify(tag, set_index, block_offset, size);
                break;
            case 'S':
                store(tag, set_index, block_offset, size);
                break;
            default:
                break;
            }
        }
        printf("\n");
        ++running_time;
    }
    
    fclose(trace_pointer);
    return 0;
}

void initCache(){
    uint32_t cache_size = (uint32_t)pow(2, set_index_bits) * lines_per_set;
    cache_ptr = malloc(sizeof(cacheLine)*cache_size);
    memset(cache_ptr, 0, cache_size*sizeof(cacheLine));
}

int main(int argc, char* argv[])
{
    // first, a para parser
    int input; 
    while ((input = getopt(argc, argv, "s:E:b:t:")) != -1)
    {
        /* code */
        switch (input)
        {
        case 's':
            sscanf(optarg, "%d", &set_index_bits);
            break;
        case 'E':
            sscanf(optarg, "%d", &lines_per_set);
            break;
        case 'b':
            sscanf(optarg, "%d", &block_bits);
            break;
        case 't':
            trace_file = optarg;
            break;

        default:
            break;
        }
    }
    printf("s=%d, E=%d, b=%d, t=%s\n", set_index_bits, lines_per_set, block_bits, trace_file);
    initCache();
    traceParser(trace_file);
    printSummary(hits, misses, evictions);
    return 0;
}
