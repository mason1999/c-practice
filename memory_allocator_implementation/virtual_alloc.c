#include <stdio.h>
#include <math.h>
#include <string.h>

#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include "functions.h"


void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {

   if (min_size > initial_size) {
      fprintf(stdout, "Error: min_size is greater than initial_size in init_allocator\n");
      return;
   }
   if (heapstart == NULL) {
      fprintf(stdout, "Error: heapstart was (NULL) in init_allocator\n");
      return;
   }

   // we begin by making enough space for our handler:
   virtual_sbrk(sizeof(struct handler));

   // we now make enough space for our data structures
   struct node* data_structures = (struct node*)virtual_sbrk(0);
   uint8_t diff = initial_size - min_size;
   virtual_sbrk(sizeof(struct node) * pow(2, diff));

   // we now make enough space for our initial block 
   char* initial_block = (char*)virtual_sbrk(0);
   virtual_sbrk(pow(2, initial_size));

   // set up the handler
   ((struct handler*)heapstart)->initial_block = initial_block;
   ((struct handler*)heapstart)->data_structures = data_structures;
   ((struct handler*)heapstart)->initial_size = initial_size;
   ((struct handler*)heapstart)->min_size = min_size;
   ((struct handler*)heapstart)->num_data_structures = diff;

   
   // go through the data structures and prepare all the nodes. That means for all of them set: index, state, size, address, next. For the 0th index 1, that
   // is the node that will be the first "free" one. All else is trivial
   struct node* temp = ((struct handler*)heapstart)->data_structures;

   double total = pow(2, ((struct handler*)heapstart)->num_data_structures);
   for (int i = 0; i < total; i++) {
      (temp + i)->index = i;
      (temp + i)->state = UNDEF;
      (temp + i)->size = MAX_UINT8;
      (temp + i)->address = NULL;
      (temp + i)->next = NULL;
      if (i == 0) {
	 (temp + i)->state = FREE;
	 (temp + i)->size = initial_size;
	 (temp + i)->address = ((struct handler*)heapstart)->initial_block;
      }
   } 
      
}

void * virtual_malloc(void * heapstart, uint32_t size) {

   if (heapstart == NULL) {
      fprintf(stdout, "Error: heapstart was (NULL) in virtual_malloc\n");
      return NULL;
   }

   // error checking size is 0
   if (size == 0) {
      fprintf(stdout, "cannot fulfil allocaiton as size entered is 0\n");
      return NULL;
   }

   // head is the head of our data structures in the form of a linked list
   struct handler* handle = (struct handler*)heapstart;
   struct node* head = handle->data_structures;

   // begin by finding the appropriate index for "size" bytes. We find this by taking the ceiling of log_2(size)
   double n = ceil(log2(size));

   // if n is smaller than the min size, n defaults to the minimum size
   if (n < handle->min_size) {
      n = handle->min_size;
   }

   // error checking that requested size is too big
   if ( n > handle->initial_size) {
      return NULL;
   }

   // 1. We Traverse the linked list and find the first free node that has the same size equal to n. If this does not work, we go to step 2 down below
   struct node* current_node = head;
   int discovered = 0;
   while (current_node->next != NULL) {
      if (current_node->size == n && current_node->state == FREE) {
	 // once the node is found, change the state and return the address
	 current_node->state = USED;
	 return (void*)current_node->address;
      }
      current_node = current_node->next;
   }

   
   // the case of when we are a single node linked list or we are at the last element. We check this element and if it is the same size, we change the status to
   // used
   if (discovered == 0) {
      if (current_node->size == n && current_node->state == FREE) {
	 // If it is the first one (in a single list) or the last one, we change the node to allocated and return the address to it. 
	 current_node->state = USED;
	 return (void*)current_node->address;
      }
   }

   // 2. Traverse the linked list and find the first free node that has size bigger than or equal to equal to n. We use a flag called found to indicate whether
   // we have actually found such a node
   int found = 0;
   struct node* temp = head;

   while (temp->next != NULL) {
      // find the node which has size>= n, which is free. remeber to set found == 1 if you do find it
      if (temp->size >= n && (temp->state) == FREE) {
	 found = 1;
	 break;
      }
      temp = temp->next;
   }

   // if you haven't found it then check the last (first in the case of a linked list with a single node) node
   if (found == 0 ) {
      // check if this node has size < n. if so, then you must return an error "canot fulfill allocation as no spots left\n"
      if (temp->size < n) {
	 return NULL;
      }
   }

   // By this point temp would be the first node which has size >= n. A node that fulfils our criteria so we start with this one. We store the index of this node
   // and the address of it. We use num as a check to see whether the size is exactly the one we want, or whether we have to halve it. 
   found = 1;
   uint8_t num = temp->size;
   char* cur_address = temp->address;
   double total_data_structures = pow(2, handle->num_data_structures);

   // eg if num = 8 and n = 4, num would increment to 7, 6, 5 and break out when n = 4
   while (num > n) {
      num = num - 1;
      temp->size = num;

      // find node that is at an index of (the current index + 2^{temp->size - min}
      double find_index = temp->index + pow(2, temp->size - handle->min_size);
      for (int i = 0; i < total_data_structures; i++) {

	if ((head + i)->index == find_index) {


	   // change: state, size, address, next
	   (head + i)->state = FREE;
	   (head + i)->size = num;
	   (head + i)->address = cur_address + (int)pow(2, num);
	   (head + i)->next = temp->next;
	   temp->next = (head + i);

	   break;
	}

      }
      
   }

   // at this point, temp->size is equal to and we now change the state to used
   temp->state = USED;

       return (void*)cur_address;
}

int virtual_free(void * heapstart, void * ptr) {

   if (heapstart == NULL) {
      fprintf(stdout, "Error: heapstart was (NULL) in virtual_free\n");
      return 1;
   }

   if (ptr == NULL) {
      fprintf(stdout, "Error: tried to free (NULL)\n");
      return 1;
   }

   // Go through linked list and find ptr. If found, check the state. If FREE, then return 1. If USED, then free it and call the coalesce function 
   // on heapstart. 
   int entered = 0;
   struct node* temp = ((struct handler*)heapstart)->data_structures;
   while (temp->next != NULL) {
      if (temp->address == (char*)ptr) {
	 if (temp->state == FREE) {
	    fprintf(stdout, "Error: tried to double free\n");
	    return 1;
	 } else if (temp->state == USED) { 
	    entered = 1;
	    temp->state = FREE;
	    coalesce(heapstart);
	    return 0;
	 }
      }
      temp = temp->next;
   }
   // case of single list or last element. In either case, we check if the ptr is equal to temp. If it is, we free it. If it isn't then we return 1
   if (temp->address == (char*)ptr && entered == 0) {
      if (temp->state == USED) {
	 temp->state = FREE;
	 coalesce(heapstart);
	 return 0;
      } else if (temp->state == FREE) {
	 printf("Error: tried to double free\n");
	 return 1;
      }
   }
    return 1;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {

   if (heapstart == NULL) {
      fprintf(stdout, "Error: heapstart was (NULL) in virtual_realloc\n");
      return NULL;
   }

   if (ptr == NULL) {
      void* address = virtual_malloc(heapstart, size);
      return address;
   }
   if (size == 0) {
      virtual_free(heapstart, ptr);
      return NULL;
   }

   struct handler* handle = (struct handler*)heapstart;

   // Let n be the size to resize to
   uint8_t n = ceil(log2(size));

   if (n < handle->min_size) {
      n = handle->min_size;
   }

   // Go through our linked list and find the first USED block with the same address as ptr
   struct node* head = handle->data_structures;
   struct node* temp = head;
   int found = 0;
   while (temp->next != NULL) {

      if (temp->address == (char*)ptr && temp->state == USED) {
	 found = 1;
	 break;
      }
      temp = temp->next;
   }

   // if here you reach the stop of being the first node in a single list, or the last node in the list
   if (found == 0) {
      if (temp->address == (char*) ptr && temp->state == USED) {
	 found = 1;
      } else {
	 return NULL;
      }
   }

   // by this point, temp->address = ptr. We divide out work into two cases. Whether the block of 2^n to resize it to is smaller than (or equal to) or bigger than 
   // the size of the node which we found. 
   if (n <= temp->size) { 
      void* store_current_info = virtual_sbrk(0);
      int32_t increment = (int32_t)pow(2, n);
      virtual_sbrk(increment);

      memcpy(store_current_info, temp->address, increment);
      virtual_free(heapstart, (void*)temp->address);
      void* new_address = virtual_malloc(heapstart, size);
      memcpy(new_address, store_current_info, increment);
      
      increment = -1 * increment;
      virtual_sbrk(increment);
      return new_address;
       
   } else {
      // 1. store current program break into a variable store_current_info
      // 2. increment virtual_sbrk by 2^{temp->size}
      // 3. memcpy an amount of 2^{temp->size} from temp->address to store_current_info
      // 4. create a pointer to the new program break and allocate enough space for all your data structures (eg virtual_sbrk(number of data structures * sizeof
      // data structure)
      // 5. loop through the current data structures and fill them in the new spot. 
      // 6. free temp->address and coalesce(heapstart)
      // 7. new address is equal to mallocing  heapstart with a size of "size"
      // 8. if new address is NULL then restore the previous data structures and decrement the program break (both the single data structure + list) and return NULL
      // 9. if new address is not NULL then decrement the program break (list first). then memcpy contents from single data structure to new_address. decrement
      // the counter by the one structure, then  return new address
      void* store_current_info = virtual_sbrk(0);
      int32_t increment = (int32_t)pow(2, temp->size);
      virtual_sbrk(increment);
      memcpy(store_current_info, temp->address, increment);

      void* store_data_list = virtual_sbrk(0);
      uint32_t num_data_structures = (uint32_t)pow(2, handle->num_data_structures);
      virtual_sbrk(num_data_structures * sizeof(struct node));
      memcpy(store_data_list, handle->data_structures, num_data_structures * sizeof(struct node));

      virtual_free(heapstart, (void*)temp->address);
      void* new_address = virtual_malloc(heapstart, size);

      if (new_address == NULL) {
	 memcpy(handle->data_structures, store_data_list, num_data_structures * sizeof(struct node));
	 virtual_sbrk(-1 * increment + num_data_structures * sizeof(struct node) * -1);
	 return NULL;
      } else {
	 memcpy(new_address, store_current_info, increment);
	 virtual_sbrk(-1 * increment + num_data_structures * sizeof(struct node) * -1);
	 return new_address;
      }

   }
    return NULL;
}

void virtual_info(void * heapstart) {

   if (heapstart == NULL) {
      fprintf(stdout, "Error: heapstart was (NULL) in virtual_info\n");
      return;
   }

   struct handler* handle = (struct handler*)heapstart;
   struct node* head = handle->data_structures;
   while(head->next != NULL) {
      if (head->state == FREE) {
	 printf("free ");
      } else if (head->state == USED) {
	 printf("allocated ");
      } else {
	 printf("Error: undetermined ");
	 return;
      }
      double size = pow(2, head->size);
      printf("%.0lf\n", size);
      head = head->next;
   }

   if (head->state == FREE) {
      printf("free ");
   } else if (head->state == USED) {
      printf("allocated ");
   } else {
      printf("Error: undetermined ");
      return;
   }
   double size = pow(2, head->size);
   printf("%.0lf\n", size);

}
