

#include <stdio.h> 
#include <stdlib.h>

typedef struct _stru_mem_list
{
    unsigned char * free_slots_bitmap; // the bitmap of free slots in this list
} STRU_MEM_LIST;

int main() {
   //unsigned char * free_slots_bitmap;
   STRU_MEM_LIST * traverse_list = NULL;
   char c = 'c';
   traverse_list = malloc(sizeof(STRU_MEM_LIST));
   traverse_list->free_slots_bitmap = malloc(0);
   traverse_list->free_slots_bitmap[0] = 0xFF ;
   traverse_list->free_slots_bitmap[1] = 0xFF ;
   traverse_list->free_slots_bitmap[0] = traverse_list->free_slots_bitmap[0] & 0x0E;
   //traverse_list->free_slots_bitmap = traverse_list->free_slots_bitmap;
   printf("Value: %x\n",  traverse_list->free_slots_bitmap[0]);
   printf("Value: %c\tSize: %ld\n", c, sizeof(char));
}



/*#include <stdio.h>

void f(int *j) {
  (*j)++;
}

int main() {
  int i = 20;
  int *p = &i;
  f(p);
  printf("i = %d\n", i);

  return 0;
}
*/

/*#include <stdio.h>

int nextPower(unsigned int x) {
   int value = 1;
   if((x & (x - 1)) == 0) {
      value = x;
   } else {
      while(value  <=  x) { 
         value = value << 1; 
      }
   }
   return  value ;
}
int main(void) {
   int i = 0;
   unsigned char bitmap[] = {0xFF, 0xF7};

   for(i = 0; i < 2; i++) {
        printf("Value: %x\t", bitmap[i]);
   }
   printf("\n");
   bitmap[2] = 0x45;
   for(i = 0; i < 3; i++) {
        printf("Value: %x\t", bitmap[i]);
   }
   printf("\n");
   bitmap[3] = 0x45;
   for(i = 0; i < 4; i++) {
        printf("Value: %x\t", bitmap[i]);
   }
   printf("\n");

  
   return 0;
}*/

 //printf("Value: %d\n", nextPower(64));

   /*int binaryInput = 0x202; 
   int bcdResult = 0;
   int shift = 0;

   printf("Binary: 0x%x (dec: %d)\n", binaryInput , binaryInput );

   while (binaryInput > 0) {
      bcdResult |= (binaryInput % 10) << (shift++ << 2);
      binaryInput /= 10;
   }

   printf("BCD: 0x%x (dec: %d)\n", bcdResult , bcdResult );*/

