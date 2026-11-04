/**
 * FILENAME: SmartMemManager.c
 * DESCRIPTION: Implement a simplified memory manager that simulates allocating and releasing regions of a fixed-size memory pool.
 * AUTHOR: Nancy Jain
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MIN_POOL 1L
#define MAX_POOL 1000000L
#define INITIAL_CAPACITY 4

typedef enum{
FIRST_FIT = 1,
BEST_FIT,
WORST_FIT
} Strategy;

typedef enum{
    BLOCK_FREE = 0,
    BLOCK_ALLOCATED = 1
} BlockStatus;

typedef struct{
    size_t offset;
    size_t size;
    int id;
    BlockStatus status;
} Block;

typedef struct{
    unsigned char *pool;
    size_t pool_size;
    Block *blocks;
    size_t count;
    size_t capacity;
    int next_id;
    Strategy strategy;
    size_t last_failed;
} MemoryManager;

static int readLong(const char *message , long *out){
    char buffer[128];
    char *end;
    long v;
    printf("%s" , message);
    fflush(stdout);
    if(!fgets(buffer , sizeof(buffer) , stdin)) return -1;
    if(!strchr(buffer , '\n')){
        int ch;
        while((ch = getchar()) != '\n' && ch != EOF) {}
        return 0;
    }
    v = strtol(buffer , &end , 10);
    if(end == buffer) return 0;
    while(*end == ' ' || *end == '\t') end++;
    if(*end != '\n' && *end != '\0') return 0;
    if(v == LONG_MAX || v == LONG_MIN) return 0;
    *out = v;
    return 1;
}

static int ensure_capacity(MemoryManager *mm , size_t needed){
    if(needed <= mm->capacity) return 1;
    size_t new_cap = mm->capacity ? mm->capacity : INITIAL_CAPACITY;
    while(new_cap < needed) new_cap *= 2;
    Block *temp = realloc(mm->blocks , new_cap * sizeof(Block));
    if(!temp) return 0;
    mm->blocks = temp;
    mm->capacity = new_cap;
    return 1;
}

static void insert_block(MemoryManager *mm , size_t index , Block b){
    memmove(&mm->blocks[index + 1] , &mm->blocks[index] , (mm->count - index) * sizeof(Block));
    mm->blocks[index] = b;
    mm->count++;
}

static void remove_block(MemoryManager *mm , size_t index){
    memmove(&mm->blocks[index] , &mm->blocks[index + 1] , (mm->count - index - 1) * sizeof(Block));
    mm->count--;
}

static long find_by_id(const MemoryManager *mm , int id){
    for(size_t i = 0 ; i < mm->count ; i++){
        if(mm->blocks[i].status == BLOCK_ALLOCATED && mm->blocks[i].id == id) return (long)i;
    }
    return -1;
}

static int validate_map(const MemoryManager *mm){
    size_t expected = 0;
    for(size_t i = 0 ; i < mm->count ; i++){
        const Block *b = &mm->blocks[i];
        if(b->status != BLOCK_FREE && b->status != BLOCK_ALLOCATED) return 0;
        if(b->offset != expected || b->size == 0) return 0;
        if(b->status == BLOCK_FREE){
            if(b->id != 0) return 0;
            if(i > 0 && mm -> blocks[i - 1].status == BLOCK_FREE) return 0;
        }else{
            if(b->id <= 0 || b->id >= mm->next_id) return 0;
            for(size_t j = 0 ; j < i ; j++){
                if(mm->blocks[j].status == BLOCK_ALLOCATED && mm->blocks[j].id == b->id) return 0;
            }
        }
        expected += b->size;
    }
    return expected == mm->pool_size;
}

static long first_fit(const MemoryManager *mm , size_t size){
    for(size_t i = 0 ; i < mm->count ; i++){
        if(mm->blocks[i].status == BLOCK_FREE && mm->blocks[i].size >= size) return (long)i;
    }
    return -1;
}

static long best_fit(const MemoryManager *mm , size_t size){
    long best = -1;
    for(size_t i = 0 ; i < mm->count ; i++){
        const Block *b = &mm->blocks[i];
        if(b->status == BLOCK_FREE && b->size >= size && (best < 0 || b-> size < mm->blocks[best].size)) best = (long)i;
    }
    return best;
}

static long worst_fit(const MemoryManager *mm , size_t size){
    long worst = -1;
    for(size_t i = 0; i < mm->count; i++){
        const Block *b = &mm->blocks[i];
        if (b->status == BLOCK_FREE && b->size >= size && (worst < 0 || b->size > mm->blocks[worst].size)) worst = (long)i;
    }
    return worst;
}

static long find_free_block(const MemoryManager *mm, size_t size){
    switch (mm->strategy){
    case BEST_FIT: return best_fit(mm, size);
    case WORST_FIT: return worst_fit(mm, size);
    default: return first_fit(mm, size);
    }
}

static const char *strategy_name(Strategy s){
    switch(s){
    case BEST_FIT: return "Best Fit";
    case WORST_FIT: return "Worst Fit";
    default: return "First Fit";
    }
}

static void merge_free_blocks(MemoryManager *mm){
    size_t i = 0;
    while (i + 1 < mm->count){
        Block *a = &mm->blocks[i];
        Block *b = &mm->blocks[i + 1];
        if(a->status == BLOCK_FREE && b->status == BLOCK_FREE){
            a->size += b->size;
            remove_block(mm, i + 1);
        }else    i++;
    }
}

typedef struct{
    size_t allocated_bytes, free_bytes;
    size_t allocated_blocks, free_blocks;
    size_t largest_free;
} Stats;

static Stats compute_stats(const MemoryManager *mm){
    Stats s = {0, 0, 0, 0, 0};
    for(size_t i = 0; i < mm->count; i++){
        const Block *b = &mm->blocks[i];
        if(b->status == BLOCK_ALLOCATED){
            s.allocated_bytes += b->size;
            s.allocated_blocks++;
        }else{
            s.free_bytes += b->size;
            s.free_blocks++;
            if (b->size > s.largest_free)   s.largest_free = b->size;
        }
    }
    return s;
}

static void report_fragmentation(const MemoryManager *mm, size_t size){
    Stats s = compute_stats(mm);
    if(s.free_bytes < size || s.largest_free >= size)  return;
    int first = 1;
    for(size_t i = 0; i < mm->count; i++){
        if(mm->blocks[i].status == BLOCK_FREE)  first = 0;
        
    }
    printf("\n");
}

static void carve_block(MemoryManager *mm, size_t idx, size_t size, int id){
    Block *b = &mm->blocks[idx];
    size_t remainder = b->size - size;
    size_t start = b->offset;
    b->size = size;
    b->id = id;
    b->status = BLOCK_ALLOCATED;
    if (remainder > 0){
        Block rest = {start + size, remainder, 0, BLOCK_FREE};
        insert_block(mm, idx + 1, rest);
    }
}

static int allocate_memory(MemoryManager *mm, size_t size){
    if(size == 0 || size > mm->pool_size){
        printf("Invalid size: must be between 1 and %zu.\n", mm->pool_size);
        return 0;
    }
    if(mm->next_id == INT_MAX){
        printf("Allocation failed: no more allocation IDs available.\n");
        return 0;
    }
    long idx = find_free_block(mm, size);
    if(idx < 0){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        report_fragmentation(mm, size);
        mm->last_failed = size;
        return 0;
    }
    if(!ensure_capacity(mm, mm->count + 1)){
        printf("Allocation failed: could not grow block metadata.\n");
        return 0;
    }
    int id = mm->next_id++;
    carve_block(mm, (size_t)idx, size, id);
    memset(mm->pool + mm->blocks[idx].offset, 'A' + (id - 1) % 26, size);
    mm->last_failed = 0;
    printf("Allocation successful. ID: %d, Start: %zu, Size: %zu\n", id, mm->blocks[idx].offset, size);
    return id;
}

static int release_memory(MemoryManager *mm, int id){
    if(id <= 0){
        printf("Invalid ID: allocation IDs are positive integers.\n");
        return 0;
    }
    long idx = find_by_id(mm, id);
    if(idx < 0){
        printf("Release failed: no allocation with ID %d exists (unknown ID or already released).\n", id);
        return 0;
    }
    mm->blocks[idx].status = BLOCK_FREE;
    mm->blocks[idx].id = 0;
    merge_free_blocks(mm);
    printf("Allocation released.\n");
    return 1;
}

static int resize_memory(MemoryManager *mm, int id, size_t new_size){
    if(id <= 0){
        printf("Invalid ID: allocation IDs are positive integers.\n");
        return 0;
    }
    if(new_size == 0 || new_size > mm->pool_size){
        printf("Invalid size: must be between 1 and %zu.\n", mm->pool_size);
        return 0;
    }
    long found = find_by_id(mm, id);
    if(found < 0){
        printf("Resize failed: no allocation with ID %d exists.\n", id);
        return 0;
    }
    size_t i = (size_t)found;
    size_t old_size = mm->blocks[i].size;
    if(new_size == old_size){
        printf("Allocation already has that size. Nothing to do.\n");
        return 1;
    }
    if(new_size < old_size){
        size_t diff = old_size - new_size;
        int next_free = (i + 1 < mm->count && mm->blocks[i + 1].status == BLOCK_FREE);
        if(!next_free && !ensure_capacity(mm, mm->count + 1)){
            printf("Resize failed: could not grow block metadata. Allocation unchanged.\n");
            return 0;
        }
        mm->blocks[i].size = new_size;
        if(next_free){
            mm->blocks[i + 1].offset -= diff;
            mm->blocks[i + 1].size += diff;
        }else{
            Block fr = { mm->blocks[i].offset + new_size, diff, 0, BLOCK_FREE };
            insert_block(mm, i + 1, fr);
        }
        merge_free_blocks(mm);
        printf("Allocation %d shrunk to %zu bytes (in place).\n", id, new_size);
        return 1;
    }
    size_t need = new_size - old_size;
    if(i + 1 < mm->count && mm->blocks[i + 1].status == BLOCK_FREE && mm->blocks[i + 1].size >= need){
        mm->blocks[i].size = new_size;
        mm->blocks[i + 1].offset += need;
        mm->blocks[i + 1].size -= need;
        if(mm->blocks[i + 1].size == 0) remove_block(mm, i + 1);
        printf("Allocation %d expanded in place to %zu bytes.\n", id, new_size);
        return 1;
    }
    long j = find_free_block(mm, new_size);
    if(j < 0){
        printf("Resize failed: cannot expand in place and no free block of %zu bytes exists. Allocation unchanged.\n", new_size);
        report_fragmentation(mm, new_size);
        return 0;
    }
    if(!ensure_capacity(mm, mm->count + 1)){
        printf("Resize failed: could not grow block metadata. Allocation unchanged.\n");
        return 0;
    }
    size_t old_off = mm->blocks[i].offset;
    size_t new_off = mm->blocks[j].offset;
    memcpy(mm->pool + new_off, mm->pool + old_off, old_size);
    memset(mm->pool + new_off + old_size, 0, new_size - old_size);
    mm->blocks[i].status = BLOCK_FREE;
    mm->blocks[i].id = 0;
    carve_block(mm, (size_t)j, new_size, id);
    merge_free_blocks(mm);
    printf("Allocation %d moved from offset %zu to %zu and resized to %zu bytes.\n", id , old_off, new_off , new_size);
    return 1;
}

static void display_map(const MemoryManager *mm){
    printf("Memory Map (%zu bytes):\n", mm->pool_size);
    for(size_t i = 0; i < mm->count; i++){
        const Block *b = &mm->blocks[i];
        if(b->status == BLOCK_ALLOCATED)    printf("[Allocated #%d: %zu bytes, Start: %zu]\n", b->id , b->size , b->offset);
        else    printf("[Free: %zu bytes, Start: %zu]\n", b->size , b->offset);
    }
}

static void display_stats(const MemoryManager *mm){
    Stats s = compute_stats(mm);
    printf("Total memory: %zu bytes\n", mm->pool_size);
    printf("Allocated: %zu bytes (%zu block%s)\n", s.allocated_bytes , s.allocated_blocks , s.allocated_blocks == 1 ? "" : "s");
    printf("Free: %zu bytes\n", s.free_bytes);
    printf("Free blocks: %zu\n", s.free_blocks);
    printf("Largest free block: %zu bytes\n", s.largest_free);
    if (mm->last_failed > 0 && s.free_bytes >= mm->last_failed && s.largest_free < mm->last_failed){  
        printf("Note: The request for %zu bytes failed even though %d bytes are free, because the free memory is fragmented.\n", mm->last_failed , s.free_bytes);
    }    
}

static int init_manager(MemoryManager *mm, size_t pool_size){
    memset(mm, 0, sizeof *mm);
    mm->pool = malloc(pool_size);
    if(!mm->pool){
        printf("Error: could not allocate the memory pool.\n");
        return 0;
    }
    mm->blocks = malloc(INITIAL_CAPACITY * sizeof(Block));
    if(!mm->blocks){
        printf("Error: could not allocate block metadata.\n");
        free(mm->pool);
        mm->pool = NULL;
        return 0;
    }
    mm->pool_size = pool_size;
    mm->capacity = INITIAL_CAPACITY;
    mm->count = 1;
    mm->blocks[0] = (Block){ 0, pool_size, 0, BLOCK_FREE };
    mm->next_id = 1;
    mm->strategy = FIRST_FIT;
    memset(mm->pool, 0, pool_size);
    return 1;
}

static void destroy_manager(MemoryManager *mm){
    free(mm->pool);
    free(mm->blocks);
    mm->pool = NULL;
    mm->blocks = NULL;
}
 
static int choose_strategy(MemoryManager *mm){
    while(1){
        long v;
        int r = readLong("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ", &v);
        if(r < 0)  return 0;
        if(r == 1 && v >= 1 && v <= 3){
            mm->strategy = (Strategy)v;
            printf("Strategy set to %s.\n", strategy_name(mm->strategy));
            return 1;
        }
        printf("Invalid choice. Enter 1, 2, or 3.\n");
    }
}

int main(void){
    MemoryManager mm;
    long v;
    int r;
    while(1){
        r = readLong("Enter memory pool size: ", &v);
        if(r < 0)   return 1;
        if (r == 1 && v >= MIN_POOL && v <= MAX_POOL)   break;
        printf("Invalid pool size: must be between %ld and %ld bytes.\n", MIN_POOL, MAX_POOL);
    }
    if(!init_manager(&mm, (size_t)v))   return 1;
    if(!choose_strategy(&mm)){
        destroy_manager(&mm);
        return 1;
    }
    while(1){
        printf("\nMenu:\n"
               "1. Allocate memory\n"
               "2. Release memory\n"
               "3. Resize allocation\n"
               "4. Display memory map\n"
               "5. Display memory statistics\n"
               "6. Change allocation strategy\n"
               "7. Exit\n");
        r = readLong("Choice: ", &v);
        if(r < 0)  break;
        if(r == 0){
            printf("Invalid input. Enter a number from 1 to 7.\n");
            continue;
        }
        long a , b;
        switch(v){
        case 1:
            r = readLong("Requested size: ", &a);
            if(r < 0) goto done;
            if(r == 0 || a <= 0 || (unsigned long)a > mm.pool_size)
                printf("Invalid size: must be between 1 and %zu.\n", mm.pool_size);
            else
                allocate_memory(&mm, (size_t)a);
            break;
        case 2:
            r = readLong("Allocation ID: ", &a);
            if(r < 0) goto done;
            if(r == 0 || a <= 0 || a > INT_MAX)
                printf("Invalid ID: must be a positive integer.\n");
            else
                release_memory(&mm, (int)a);
            break;
        case 3:
            r = readLong("Allocation ID: ", &a);
            if(r < 0) goto done;
            if(r == 0 || a <= 0 || a > INT_MAX){
                printf("Invalid ID: must be a positive integer.\n");
                break;
            }
            r = readLong("New size: ", &b);
            if(r < 0) goto done;
            if(r == 0 || b <= 0 || (unsigned long)b > mm.pool_size)
                printf("Invalid size: must be between 1 and %zu.\n", mm.pool_size);
            else
                resize_memory(&mm, (int)a, (size_t)b);
            break;
        case 4:
            display_map(&mm);
            break;
        case 5:
            display_stats(&mm);
            break;
        case 6:
            if(!choose_strategy(&mm)) goto done;
            break;
        case 7:
            goto done;
        default:
            printf("Invalid choice. Enter a number from 1 to 7.\n");
        }
        if (!validate_map(&mm))
            printf("INTERNAL ERROR: memory map is inconsistent!\n");
    }
done:
    destroy_manager(&mm);
    printf("Goodbye.\n");
    return 0;
}