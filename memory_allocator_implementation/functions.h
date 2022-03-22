#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>

#define FREE 1
#define USED 0
#define UNDEF 2
#define MAX_UINT8 255

/* The role of this struct is to store the addresses of the initial_block section and the data structure section
 * initial_block = address of initial block
 * data_structures = pointer to first element in data structures
 * initial_size = initial_size provided (power)
 * min_size = min_size provided (power)
 * index_data_structures = initial_size - min_size (power)
 */

struct handler {
   struct node* data_structures;
   char* initial_block;
   uint8_t initial_size;
   uint8_t min_size;
   uint8_t num_data_structures;

};

/* nodes will have:
 * index: current index of the node in the list. Goes from {0,...,2^{initial_size - min_size} - 1}
 * state: either free or used
 * size: current size index. 2^{size} will be the actual size
 * max: the intial size index
 * min: the smallest size index
 * next: a pointer to the next node in our list. 
 */
struct node {
   uint32_t index;
   uint8_t state;
   uint8_t size;
   char* address;
   struct node* next;

};

// given the heapstart pointer, this function will coalesce any free nodes and change the linked list that we have. 
void coalesce(void* heapstart);
#endif
