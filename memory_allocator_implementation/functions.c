#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include "functions.h"

void coalesce(void* heapstart) {
   uint8_t min_size = ((struct handler*)heapstart)->min_size;
   struct node* head = ((struct handler*)heapstart)->data_structures;
   struct node* current_node = head;
   struct node* next_node = head->next;

   while(next_node != NULL) {
      // 1st check: check if both nodes are free
      if (current_node->state == FREE && next_node->state == FREE) {
	 // 2nd check: check if both nodes are the same size
	 if (current_node->size == next_node->size) {
	    // 3rd check: check if the current node is a leader
	    if ( (current_node->index % (uint32_t)pow(2, current_node->size - min_size + 1)) == 0 ) {
	       // 4th check: Check if the next node has index curren_node index + (some particular amount)
	       if (next_node->index == (current_node->index + (uint32_t)pow(2, current_node->size - min_size))) {
		  // If we reach here this is the step where we have to coalesce. The reason we reset current_node and next_node is because we want to check
		  // again. If this is never entered, we will leave the loop and be done. 
		  current_node->next = next_node->next;
		  current_node->size = current_node->size + 1;
		  current_node = head;
		  next_node = head->next;
		  continue;
	       }
	    }
	 }
      }
      current_node = current_node->next;
      next_node = next_node->next;
   }
}
