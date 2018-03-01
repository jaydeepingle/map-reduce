/*
 * Binghamton CS 451/551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
#include "interposition.h"

int main(int argc, char * argv[])
{
    mem_mngr_init();

    STRU_MEM_LIST * mem_p4 = NULL;
    STRU_MEM_LIST * mem_p1 = NULL;
    STRU_MEM_LIST * mem_p2 = NULL;
    STRU_MEM_LIST * mem_p3 = NULL;
    STRU_MEM_LIST * mem_p5 = NULL;

    mem_p1 = mem_mngr_alloc(1);
    mem_p2 = mem_mngr_alloc(1);
    mem_p3 = mem_mngr_alloc(1);
    mem_p4 = mem_mngr_alloc(1);

    mem_mngr_free(mem_p2);
    mem_p5 = mem_mngr_alloc(1);
    mem_mngr_print_snapshot();
    mem_mngr_leave();
    return 0;
}


