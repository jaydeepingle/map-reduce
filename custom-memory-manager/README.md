Name 	: Jaydeep Digambar Ingle
B Number: B00671052
Email ID: jingle1@binghamton.edu
Status	: Complete
--------------------------------------------------------------------------------
Description:
There are methods for allocating and freeing out the memory.

Logs:

1. 
mem_p = mem_mngr_alloc(1);
mem_p = mem_mngr_alloc(9);
mem_p = mem_mngr_alloc(11);
mem_p = mem_mngr_alloc(32);

$ make run 
./a.out


mem_pool null: 1	alloc_size: 8

bitmap 0x1877450 size 1 bytes: 1111  1111  


bitmap 0x1877450 size 1 bytes: 0111  1111  




mem_pool not null: 9	alloc_size: 16
mem_pool doesn't contain the required list: 9	alloc_size: 16

bitmap 0x1877510 size 1 bytes: 1111  1111  


bitmap 0x1877510 size 1 bytes: 0111  1111  




mem_pool not null: 11	alloc_size: 16
mem_pool contains the required list: 11	alloc_size: 16

bitmap 0x1877510 size 1 bytes: 0111  1111  


bitmap 0x1877510 size 1 bytes: 0111  1111  

free slot available in the bitmap: 11	alloc_size: 16

bitmap 0x1877510 size 1 bytes: 0111  1111  


bitmap 0x1877510 size 1 bytes: 0011  1111  




mem_pool not null: 32	alloc_size: 32
mem_pool doesn't contain the required list: 32	alloc_size: 32

bitmap 0x1877610 size 1 bytes: 1111  1111  


bitmap 0x1877610 size 1 bytes: 0111  1111  




2.
mem_p1 = mem_mngr_alloc(1);
mem_p2 = mem_mngr_alloc(1);
mem_p3 = mem_mngr_alloc(1);
mem_p4 = mem_mngr_alloc(1);

mem_mngr_free(mem_p2);
mem_p5 = mem_mngr_alloc(1);



$ make run 
./a.out


mem_pool null: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 1111  1111  


bitmap 0x18d1450 size 1 bytes: 0111  1111  




mem_pool not null: 1	alloc_size: 8
mem_pool contains the required list: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0111  1111  


bitmap 0x18d1450 size 1 bytes: 0111  1111  

free slot available in the bitmap: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0111  1111  


bitmap 0x18d1450 size 1 bytes: 0011  1111  




mem_pool not null: 1	alloc_size: 8
mem_pool contains the required list: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0011  1111  


bitmap 0x18d1450 size 1 bytes: 0011  1111  

free slot available in the bitmap: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0011  1111  


bitmap 0x18d1450 size 1 bytes: 0001  1111  




mem_pool not null: 1	alloc_size: 8
mem_pool contains the required list: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0001  1111  


bitmap 0x18d1450 size 1 bytes: 0001  1111  

free slot available in the bitmap: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0001  1111  


bitmap 0x18d1450 size 1 bytes: 0000  1111  



bitmap 0x18d1450 size 1 bytes: 0000  1111  


bitmap 0x18d1450 size 1 bytes: 0010  1111  



mem_pool not null: 1	alloc_size: 8
mem_pool contains the required list: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0010  1111  


bitmap 0x18d1450 size 1 bytes: 0010  1111  

free slot available in the bitmap: 1	alloc_size: 8

bitmap 0x18d1450 size 1 bytes: 0010  1111  


bitmap 0x18d1450 size 1 bytes: 0000  1111 

--------------------------------------------------------------------------------

Files:

• bitmap.c
• common.h
• interposition.h
• memory manager.c
• memory manager.h
• Makefile
• README

--------------------------------------------------------------------------------