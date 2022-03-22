#include <stdlib.h>
#include <stdio.h>
#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include "functions.h"

void * virtual_heap = NULL;
int32_t my_program_break = 0;

// When you call virtual_sbrk, given an increment, it returns a pointer to your virtual_heap
void * virtual_sbrk(int32_t increment) {
   // Your implementation here (for your testing only)

   if (virtual_heap == NULL) {
      fprintf(stderr, "Error virtual_heap doens't point to any space\n");
   }

   my_program_break = my_program_break + increment;

   if (my_program_break < 0) {
      fprintf(stderr, "Error you cannot decrement to before heap space\n");
      return (void*)(-1);
   }

   if (my_program_break > INT32_MAX) {
      fprintf(stderr, "Error you cannot increment past the whole heap\n");
      return (void*)(-1);
   }

   char* temp = (char*)virtual_heap + my_program_break;
   return (void*)temp;

}

int main() {

   // Your own testing code here

   // We malloc the maximum amount available 
   virtual_heap = malloc(INT32_MAX);

init_allocator(virtual_heap, 18, 11);
virtual_malloc(virtual_heap, 4095);
virtual_malloc(virtual_heap, 2048);
virtual_malloc(virtual_heap, 5000);
virtual_info(virtual_heap);

virtual_free(virtual_heap, (((struct handler*)virtual_heap)->data_structures)->address);
virtual_info(virtual_heap);

virtual_malloc(virtual_heap, 2048);
virtual_info(virtual_heap);

virtual_free(virtual_heap, ((((((struct handler*)virtual_heap)->data_structures)->next)->next)->next)->address);
virtual_info(virtual_heap);
virtual_free(virtual_heap, ((((struct handler*)virtual_heap)->data_structures)->next)->address);
virtual_info(virtual_heap);
virtual_free(virtual_heap, (((((struct handler*)virtual_heap)->data_structures)->next)->next)->address);
virtual_info(virtual_heap);
    return 0;

}
