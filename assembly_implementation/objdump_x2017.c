#include <stdio.h>
#include <stdlib.h>
#include "./headerfiles/functions.h"
#include "./headerfiles/list.h"

// we use this to read in the binary from a path in the computer
int main(int argc, char** argv) {
   // We read from file into our file buffer
   unsigned char* file_buffer = NULL;
   int buffer_size = 0;

   if (argc  < 2) {
      printf("error\n");
      return 1;
   } else {

      // reads into the buffer file_buffer with the buffer_size
      read_into_buffer(argv[1], &file_buffer, &buffer_size); // sets file_buffer and buffer_size

      
      // set up program memory
      struct inst* head = NULL; 
      create_program_memory(file_buffer, buffer_size, &head);
      free(file_buffer); // FREE FILE BUFFER
      init_indices(head);
      //print_list_formatted_num(head);

      // set up function table
      unsigned char ft[8];
      init_function_table(head, ft, 8);

      // set up mapping of each function in the order the variables comes in 
      struct mapping* map = NULL;
      create_mapping(&map);
      print_list_formatted_with_mapping(head, map);
     // printf("\n\n");
      //init_map(head, ft, map);
      //print_map(map);

      free_list(&head); // FREE PROGRAM MEMORY
      free(map); // FREE MAP

   }

   return 0;
}
