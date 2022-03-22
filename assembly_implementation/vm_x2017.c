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

      /////////////////////////////////////////////////////////////////////// SET UP //////////////////////////////////////////////////////////////////////////////
      // reads into the buffer file_buffer with the buffer_size
      read_into_buffer(argv[1], &file_buffer, &buffer_size); // sets file_buffer and buffer_size
      
      // set up program memory
      struct inst* head = NULL; 
      create_program_memory(file_buffer, buffer_size, &head);
      free(file_buffer); // FREE FILE BUFFER
      init_indices(head);

      // set up function table
      unsigned char ft[8];
      init_function_table(head, ft, 8);

      // set up mapping of each function in the order the variables comes in 
      struct mapping* map = NULL;
      create_mapping(&map);

      // set up the registers and stack and initialise them all to zero
      unsigned char stack[MAX_RAM];
      unsigned char registers[8];
      for (int i = 0; i < MAX_RAM; i++) {
	 stack[i] = 0;
      }
      for (int i = 0; i < 8; i++) {
	 registers[i] = 0;
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // At this point we have 
      // 1. head: linked list which is the functional program memory
      // 2. ft: an array whose contents give the instruction number of functions which appeared. 
      // 3. map: a struct where we provide the mappings from binary to our code, dependent on function currently executing.  
      // 4. stack: an array whose size is 256
      // 5. registers: 8 registers implemented as an array of 8 unsigned bytes
      // (i) register 4 is for keeping track of what function we're currently executing. current function should initially be 0
      // (ii) register 5 keeps track of the bottom of the current stack frame. we call this bp. iniital value is 0. It always holds the "address" (index) of the 
      // first element in the stack frame
      // (iii) register 6 keeps track of the top of the current stack frame. we call this sp. initial value is 0. It always points to "address" (index) of the
      // free space on top of the stack
      // (iv) register 7 keeps is the program counter and keeps track of the instruction number. 

     
      // START
      // Idea: find the instruction number corresonding to return in function zero and keep executing instructions until you reach it. We divide into three
      // cases: when the instruction is a CAL, RET or any other instruction. Note that I was not able to implement CAL or RET properly because it just kept
      // stuffing up.Though in theory:
      // (i) CAL: push the base pointer (bp), registers (r0, r1, r2, r3), return address (r7) and the current function being executed (r4) onto the stack while
      // incrementing the stack pointer (sp) to always "point" to the first empty position. Next change the value of bp to sp and consider this your new stack
      // frame
      // (ii) RET: decrement the stack pointer (sp) until you get to bp (base pointer). Now pop off the current function and put it into r4, pop off the return 
      // address and put it into r7, pop of r3, r2, r1 and r0 and restore the values and finally pop off the value of the old base pointer and put it back into 
      // bp. 
      
      unsigned char ret_0_instruction_number = 0;
      find_f0_ret(head, &ret_0_instruction_number);

      struct inst* h = NULL;
      find_function(&h, head, ft[0]);		      // find instruction number corresponding to function label 0
      registers[7] = h->ins_num;		      // update program counter to instruction of 0 function

      while ( registers[7] != ret_0_instruction_number ) {   // while the current opcode is not a RET for f_0

	 struct inst * temp = h;		      // store the instruction that you want to do
	 
	 h = h->next;				      // go to next instruction
	 registers[7] = h->ins_num;		      // update program counter
	 unsigned char cur_ins_num = registers[7];    // save the current instruction number after incrementing (see 1. below)

	 if (temp->opcode == 1) {		      // CAL 

	    // tried implementing it, but the program suddenly would not work

	 } else if (temp->opcode == 2) {	      // RET


	    // tried implementing it but the program would suddenly not work

	 } else {				      // any other instruction

	    int check = execute(temp, map, stack, registers, registers[4]); // execute the current instruction
	    if (check == 1) {
	       printf("ERROR\n");
	       free_list(&head);
	       free(map);
	       return 1;
	    }

	    if (registers[6] >= MAX_BYTE_SIZE) {	       // stack overflow
	       free_list(&head);
	       free(map);
	       return 1;
	    }
	    if (cur_ins_num != registers[7]) {		       // 1. if after execute you have jumped elsewhere in code, you have to keep this in mind...
	       find_function(&h, head, registers[7]);	       // though this was to find functions, we can use it to find any instruction (poorly named though)
	    }

	 }

      }  
      free_list(&head); // FREE PROGRAM MEMORY
      free(map); // FREE MAP

   }

   return 0;
}
