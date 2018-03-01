	/*
 * Binghamton CS 451/551 Project "Memory manager".
 * This file needs to be turned in.	
 */


#include "memory_manager.h"

static STRU_MEM_LIST * mem_pool = NULL;

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager organizes the memory.
 * Do not change the implementation of this function. It will be used to help the grading.
 */
void mem_mngr_print_snapshot(void)
{
    STRU_MEM_LIST * mem_list = NULL;

    printf("============== Memory snapshot ===============\n");

    mem_list = mem_pool; // Get the first memory list
    while(NULL != mem_list)
    {
        STRU_MEM_BATCH * mem_batch = mem_list->first_batch; // Get the first mem batch from the list 

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n", 
                   mem_list, mem_list->slot_size, mem_list->batch_count, mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->bitmap_size);

        while (NULL != mem_batch)
        {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch, mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch; // get next mem batch
        }

        mem_list = mem_list->next_list;
    }

    printf("==============================================\n");
}

// This function adds a batch to the memory list
void add_batch(STRU_MEM_BATCH * batch, int alloc_size) {
    batch->batch_mem = malloc(MEM_BATCH_SLOT_COUNT * alloc_size);
    batch->next_batch = NULL;
}

// It creates a new instance of memory list and returns
void add_to_mem_pool(STRU_MEM_LIST * list, int alloc_size) {
    int i = 0, malloc_size = (int)(MEM_BATCH_SLOT_COUNT / 8);
    list->slot_size = alloc_size;
    list->batch_count = 1;
    list->free_slots_bitmap = malloc(malloc_size); // TO DO: Fix this
    // IMPORTANT
    for(i = 0; i < malloc_size; i++) {
        list->free_slots_bitmap[i] = ~(0x0);    
    }
    list->bitmap_size = 1;
    list->first_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
    add_batch(list->first_batch, alloc_size);
    list->next_list = NULL;
}

/*
 * Initialize the memory manager.
 * You may add your code related to initialization here if there is any.
 */
void mem_mngr_init(void)
{
    mem_pool = NULL;
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void)
{
    STRU_MEM_LIST * temp_list = mem_pool;
    STRU_MEM_LIST * nested_temp_list = mem_pool;
    STRU_MEM_BATCH * temp_batch = NULL;
    STRU_MEM_BATCH * nested_temp_batch = NULL;
    
    while(temp_list != NULL) {
        temp_batch = temp_list->first_batch;    
        while(temp_batch != NULL) {
            nested_temp_batch = temp_batch;
            temp_batch = temp_batch->next_batch;
            // Freeing up the batch memory in each batch
            if(nested_temp_batch != NULL) {
                if(nested_temp_batch->batch_mem != NULL) {
                    free(nested_temp_batch->batch_mem);
                    nested_temp_batch->batch_mem = NULL;
                }
                free(nested_temp_batch);
                nested_temp_batch = NULL;
            }
        }
        nested_temp_list = temp_list;
        temp_list = temp_list->next_list;
        if(nested_temp_list != NULL) {
            free(nested_temp_list);
            nested_temp_list = NULL;
        }
    }
}

/*
 * Allocate a chunk of memory 	
 */
void * mem_mngr_alloc(size_t size)
{
    /*
    1. Check whether mem_pool is null
        2. if mem_pool == null, init mem_pool with the alloc_size
        3. if mem_pool != null, traverse through all the lists and check for the required slot size
            4. if alloc_size == required_size, search for the free slot within the bitmap
                5. if free slot found return the pointer else allocate new memory and return the pointer to the start
                6. if no free slot found, add new batch and allocate and return the pointer
            7. if alloc_size != required_size, create new mem_list and mem_batch and allocate chunk and attach
    */
    int alloc_size = SLOT_ALLINED_SIZE(size), location = 0;
    int bitmap_count = (MEM_BATCH_SLOT_COUNT / 8);
    STRU_MEM_LIST * iterator = mem_pool;
    STRU_MEM_LIST * prev_list = NULL;
    STRU_MEM_LIST * next_mem = NULL;
    STRU_MEM_BATCH * return_batch = NULL;
    STRU_MEM_BATCH * new_batch = NULL;
    STRU_MEM_BATCH * temp_batch = NULL;
    STRU_MEM_BATCH * prev_batch = NULL;
    if(iterator == NULL) {
        // mem pool null
        iterator = (STRU_MEM_LIST*)malloc(sizeof(STRU_MEM_LIST));
        // This method will take care of batch_count and bitmap_size
        add_to_mem_pool(iterator, alloc_size); 

        bitmap_clear_bit(iterator->free_slots_bitmap, bitmap_count, 0); //use the logic of clearing the bit or setting the bit
        
        iterator->next_list = NULL;
        mem_pool = iterator;
        return_batch = iterator->first_batch->batch_mem;
    } else {
        // mem pool not null
        prev_list = iterator;
        while(iterator->slot_size != alloc_size && iterator->next_list != NULL) {
            iterator = iterator->next_list;
        }
        // If the list of required size is not found
        if(iterator->slot_size != alloc_size && iterator->next_list == NULL) {  //end
            // If the list of required size is not found
            next_mem = (STRU_MEM_LIST *)malloc(sizeof(STRU_MEM_LIST));
            // This method will take care of batch_count and bitmap_size
            add_to_mem_pool(next_mem, alloc_size);
            
            bitmap_clear_bit(next_mem->free_slots_bitmap, bitmap_count, 0);

            // if slot of size 63 and alignment is of 8 bytes then, alloc 64 bytes using 8 bytes alignment
            // then slot of 64 which is multiple of 8, if it was 128 then 128
            next_mem->next_list = NULL;
            iterator->next_list = next_mem;
            return_batch = next_mem->first_batch->batch_mem;
            iterator = mem_pool;
        } else if(iterator->slot_size == alloc_size) {
            
            // check bit map and look for the free slot and return the pointer after allocation
            // call bitmap bitmap_find_first_bit and get the index and return (pointer + Number)
            // call the clear the index bit after and return the pointer
            
            location = bitmap_find_first_bit(iterator->free_slots_bitmap, bitmap_count, 1); //sizeof(iterator->free_slots_bitmap)
            
            if(location == -1) {
                int malloc_size = (int)(MEM_BATCH_SLOT_COUNT / BIT_PER_BYTE);
                int bit_clear = iterator->batch_count * MEM_BATCH_SLOT_COUNT;
                new_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
                add_batch(new_batch, alloc_size);
                temp_batch = iterator->first_batch;
                while(temp_batch != NULL) {
                    prev_batch = temp_batch;
                    temp_batch = temp_batch->next_batch;
                }
                
                iterator->batch_count = iterator->batch_count + 1;
                iterator->bitmap_size = iterator->bitmap_size + 1;

                // TO DO: Increase the size of free_slots_bitmap and copy the previous data too and clear the first bit
                // IMPORTANT
                iterator->free_slots_bitmap = realloc(iterator->free_slots_bitmap, iterator->batch_count);
                
                // IMPORTANT : what if the bitmap is 16
                for(int i = 0; i < (MEM_BATCH_SLOT_COUNT / BIT_PER_BYTE); i++) {
                    iterator->free_slots_bitmap[iterator->bitmap_size + i] = ~(0x0);
                }
                
                bitmap_clear_bit(iterator->free_slots_bitmap, bitmap_count, bit_clear);
                
                prev_batch->next_batch = new_batch->batch_mem;
                return_batch = new_batch->batch_mem;
            } else {
                // Add batch if didn't get the clear bit from the bitmap
                // If got the empty slot return the pointer immidiately and change the bitmap
                bitmap_clear_bit(iterator->free_slots_bitmap, bitmap_count, location);

                // IMPORTANT make sure it points to the required location
                return_batch = (iterator->first_batch->batch_mem + location); 
            }
        }
    }
    return return_batch;
}

/*
 * Free a chunk of memory pointed by ptr
 */
void mem_mngr_free(void * ptr)
{
	STRU_MEM_LIST * temp_list = NULL;
    STRU_MEM_BATCH * temp_batch = NULL;
    int index = 0, i = 0, batch_count = (MEM_BATCH_SLOT_COUNT / BIT_PER_BYTE);
    temp_list = mem_pool;
    while(temp_list != NULL) {
        temp_batch = temp_list->first_batch;
        // Handles the batches from second batch onwards
        while(temp_batch != NULL) {
            for(i = 0; i < MEM_BATCH_SLOT_COUNT; i++) {
                index++;
                if(ptr != NULL && ptr == (temp_batch->batch_mem + i)) {
                    bitmap_set_bit(temp_list->free_slots_bitmap, batch_count, index);
                }
            }
            temp_batch = temp_batch->next_batch;
            index = index + MEM_BATCH_SLOT_COUNT;
        }
        temp_list = temp_list->next_list;
        index = 0;
    }   
}
