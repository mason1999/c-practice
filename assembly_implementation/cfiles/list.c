#include <stdio.h>
#include <stdlib.h>
#include "../headerfiles/functions.h"
#include "../headerfiles/list.h"

// 1. function creates an inst struct and returns a pointer to it
struct inst* create(unsigned char opcode, unsigned char t1, unsigned char v1, unsigned char t2, unsigned char v2) {

   struct inst* ptr_ins = (struct inst*)malloc(sizeof(struct inst));
   ptr_ins->opcode = opcode;
   ptr_ins->t1 = t1;
   ptr_ins->v1 = v1;
   ptr_ins->t2 = t2;
   ptr_ins->v2 = v2;
   ptr_ins->ins_num = 0;
   ptr_ins->next = NULL;
   return ptr_ins;
   
}

// 2. function pushes to the head of an inst pointer. Need address of head because value of head will change
void push(struct inst** head, unsigned char opcode, unsigned char t1, unsigned char v1, unsigned char t2, unsigned char v2) {
   struct inst* temp = create(opcode, t1, v1, t2, v2);
   if (*head == NULL ) {
      (*head) = temp;
   } else {
      struct inst* a = *head;
      *head = temp;
      temp->next = a;
   }
}


// 3. function pops the first node
void pop(struct inst** head) {
   if (*head == NULL) {
      printf("list empty\n");
   } else {
      struct inst* temp = (*head)->next;
      free(*head);
      *head = temp;
   }
}

// 4. frees all the associated memory
void free_list(struct inst** head) {
   while (*head != NULL ) {
      pop(head);
   }
}

// 5. prints an inst struct
void print_single(struct inst * head) {
   if (head == NULL) {
      printf("NULL\n");
   } else {
      printf("opcode is %u\n", head->opcode);
      printf("t1 is %u and v1 is %u\n", head->t1, head->v1);
      printf("t2 is %u and v2 is %u\n", head->t2, head->v2);
      printf("instruction number is: %u and next instruction has address %p\n", head->ins_num, head->next);
   }
}

// 6. prints whole list of instructions
void print_list(struct inst* head) {
   while(head != NULL) {
      print_single(head);
      head = head->next;
   }
}

// TO DO:  ACCOUNT FOR lower case letters in case 0 (MOV) cause I ceebsed doing it. 
// 7. creates a linked list of instructions from a file buffer
void create_program_memory(unsigned char* file_buffer, int buffer_size, struct inst** head) {
   unsigned char keep_reading = GO;

   unsigned char num_ins = read_and_shift_right(file_buffer, buffer_size, 5);
   // at the function level
   while (keep_reading == GO) {
      // read the first 5 bits
      unsigned char i = num_ins;

      // go through all the opcodes now
      for (; i > 0; i--) {

	 // read the next 3 bits after that
	 unsigned char opcode = read_and_shift_right(file_buffer, buffer_size, 3);
	 unsigned char t1 = MAX_BYTE_SIZE;
	 unsigned char v1 = MAX_BYTE_SIZE;
	 unsigned char t2 = MAX_BYTE_SIZE;
	 unsigned char v2 = MAX_BYTE_SIZE;
	 switch(opcode)
	 {

	    case 0 : // MOV A B

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       switch(t1)
	       {
		  case 1 : // t1 is a register
		     
		     v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		     break;
		  case 2 : // t1 is a stack symbol

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     break;
		  case 3: // t1 is a pointer symbol

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     break;
		  default: 
		     printf("default entered for MOV t1\n");
		     break;
	       }

	       t2 = read_and_shift_right(file_buffer, buffer_size, 2);
	       switch(t2)
	       {
		  case 0 : // t2 is a value

		     v2 = read_and_shift_right(file_buffer, buffer_size, 8);
		     break;

		  case 1 : // t2 is a register

		     v2 = read_and_shift_right(file_buffer, buffer_size, 3);
		     break;
		  
		  case 2: // t2 is a stack symbol

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     break;
		  case 3: // t2 is a pointer symbol

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     break;
		  default: 
		     printf("default entered for MOV t2\n");
		     break;
	       }
	       // create the insruction here
	       push(head, opcode, t1, v1, t2, v2);
	       break;

	    case 1 : // CAL A 

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t1 != 0) {
		  printf("error binary wrong\n");
	       } else {
		  // t1 is VAL
		  v1 = read_and_shift_right(file_buffer, buffer_size, 8);
		  if (v1 > 7) {
		     printf("Invalid function in binary\n");
		  } 
		  // read in v1
	       }
	       // create instruction here
	       push(head, opcode, t1, v1, MAX_BYTE_SIZE, MAX_BYTE_SIZE);
	       break;

	    case 2 : // RET 
	       // create instruction here
	       push(head, opcode, MAX_BYTE_SIZE, MAX_BYTE_SIZE, MAX_BYTE_SIZE, MAX_BYTE_SIZE);
	       break;

	    case 3 : // REF A B: A can be pointer valued, stack valued or register valued. IT CANNOT BE A VALUE. B can only be stack or pointer valued (not reg). 

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2); // t1 can be ptr, stack or register
	       switch(t1)
	       {
		  case 1: // t1 is REG

		     v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		     break;

		  case 2: // t1 is STK

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     break;

		  case 3: // t1 is PTR 

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     break;

		  default: 
		     printf("Entered default for REF t1\n");
		     break;
	       }

	       t2 = read_and_shift_right(file_buffer, buffer_size, 2); // CANNOT have register symbol. 
	       switch(t2)
	       {
		  case 2: // t2 is STK

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5);
		     break;

		  case 3: // t2 is PTR 

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5);
		     break;

		  default: 
		     printf("invalid symbol in REF t2\n");
		     break;
	       }
	       // create struct here
	       push(head, opcode, t1, v1, t2, v2);

	       break;

	    case 4 : // ADD A B 

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if ( t1 != 1) {
		  printf("error in binary for add in t1\n");
	       } else { 
		  // t1 must be REG
		  v1 = read_and_shift_right(file_buffer, buffer_size, 3);
	       }

	       t2 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t2 != 1) {
		  printf("error in binary for add in t2\n");
	       } else {
		  // t2 must be REG

		  v2 = read_and_shift_right(file_buffer, buffer_size, 3);
	       }
	       // create struct here
	       push(head, opcode, t1, v1, t2, v2);

	       break;

	    case 5 : // PRINT A 

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       switch(t1)
	       {
		  case 0: // t1 is VAL

		     v1 = read_and_shift_right(file_buffer, buffer_size, 8);
		     break;
		  case 1: // t1 is REG

		     v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		     break;

		  case 2: // t1 is STK 

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     break;

		  case 3:  // t1 is PTR 

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     break;
	       }
	       // create struct here
	       push(head, opcode, t1, v1, MAX_BYTE_SIZE, MAX_BYTE_SIZE);

	       break;

	    case 6 : // NOT A 

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t1 != 1) {
		  printf("error in binary in NOT t1\n");
	       } else {
		  // t1 must be REG
		  v1 = read_and_shift_right(file_buffer, buffer_size, 3);

	       }
	       // create struct here
	       push(head, opcode, t1, v1, MAX_BYTE_SIZE, MAX_BYTE_SIZE);

	       break;

	    case 7 : // EQU A 

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t1 != 1) {
		  printf("error in binary in EQU t1\n");
	       } else {
		  // t1 must be REG
		  v1 = read_and_shift_right(file_buffer, buffer_size, 3);
	       }
	       // create struct here
	       push(head, opcode, t1, v1, MAX_BYTE_SIZE, MAX_BYTE_SIZE);

	       break;

	 }
      }

      // read in the next 3 bits for the function label
      unsigned char fun_lab = read_and_shift_right(file_buffer, buffer_size, 3);
      // create struct here
      push(head, fun_lab + 8, MAX_BYTE_SIZE, MAX_BYTE_SIZE, MAX_BYTE_SIZE, MAX_BYTE_SIZE);

      // check the next 5 bits. if they are non zero we repeat. if they are zero you stop. 
      num_ins = read_and_shift_right(file_buffer, buffer_size, 5);
      if (num_ins == 0) {
	 keep_reading = STOP; 
      }
   }
}

void print_list_formatted(struct inst* head) {
  while(head != NULL) {
     // TO DO 
     unsigned char opcode = head->opcode;
     unsigned char t1 = head->t1;
     unsigned char v1 = head->v1;
     unsigned char t2 = head->t2;
     unsigned char v2 = head->v2;

     switch (opcode)
     {
	case 0: // MOV A B
	   printf("    MOV ");
	   // t1 could be REG, STK PTR
	   switch (t1)
	   {
	      case 1: // REG
		 printf("REG %u ", v1);
		 break;
	      case 2: // STK
		 printf("STK %c ", v1 + 65);
		 break;
	      case 3: // PTR
		 printf("PTR %c ", v1 + 65);
		 break;
	      default: 
		 printf("incorrect type read in MOV t1\n");
		 break;
	   }
	   // t2 could be VAL, REG, STK, PTR
	   switch (t2) 
	   {
	      case 0: // VAL
		 printf("VAL %u\n", v2);
		 break;
	      case 1: // REG
		 printf("REG %u\n", v2);
		 break;
	      case 2: // STK
		 printf("STK %c\n", v2 + 65);
		 break;
	      case 3: // PTR
		 printf("PTR %c\n", v2 + 65);
		 break;
	      default: 
		 printf("incorrect type read in MOV t2\n");
		 break;
	   }

	   break;
	case 1: // CAL A
	   printf("    CAL ");
	   switch (t1)
	   {
	      case 0: // VAL
		 printf("VAL %u\n", v1);
		 break;
	      default: // should never enter here
		 printf("incorrect type read in\n");
		 break;
	   }
	   break;
	case 2: // RET
	   printf("    RET\n");
	   break;
	case 3: // REF A B 
	   printf("    REF ");
	   switch (t1) // REG, STK, PTR
	   {
	      case 1:
		 printf("REG %u ", v1);
		 break;
	      case 2:
		 printf("STK %c ", v1 + 65);
		 break;
	      case 3:
		 printf("REG %c ", v1 + 65);
		 break;
	      default:
		 printf("REF t1 invalid binary\n");
		 break;
	   }
	   switch (t2) // STK, PTR
	   {
	      case 2:
		 printf("STK %c\n", v2 + 65);
		 break;
	      case 3:
		 printf("PTR %c\n", v2 + 65);
		 break;
	      default:
		 printf("REF t2 invalid binary\n");
		 break;
	   }
	   break;
	case 4: // ADD A B 
	   printf("    ADD ");
	   switch (t1) // REG
	   {
	      case 1:
		 printf("REG %u ",v1);
		 break;
	      default:
		 printf("ADD t1 invalid binary\n");
		 break;
	   }
	   switch (t2) // REG
	   {
	      case 1:
		 printf("REG %u\n",v2);
		 break;
	      default:
		 printf("ADD t2 invalid binary\n");
		 break;
	   }
	   break;
	case 5: // PRINT A
	   printf("    PRINT ");
	   switch (t1) // VAL, REG, STK, PTR
	   {
	      case 0:
		 printf("VAL %u\n", v1);
		 break;
	      case 1:
		 printf("REG %u\n", v1);
		 break;
	      case 2:
		 printf("STK %c\n", v1 + 65);
		 break;
	      case 3:
		 printf("PTR %c\n", v1 + 65);
		 break;
	   }
	   break;
	case 6: // NOT A
	   printf("    NOT ");
	   switch (t1)
	   {
	      case 1: 
		 printf("REG %u\n", v1);
		 break;
	      default: 
		 printf("error in NOT t1 binary\n");
		 break;
	   }
	   break;
	case 7: // EQU A

	   printf("    EQU ");
	   switch (t1)
	   {
	      case 1: 
		 printf("REG %u\n", v1);
		 break;
	      default: 
		 printf("error in NOT t1 binary\n");
		 break;
	   }
	   break;
	default: // functions and return addresses
	   if (opcode == 16) {
	      // it's a return address
	   } else {
	      // it's a function label
	      switch (opcode)
	      {
		 case 8:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 9:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 10:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 11:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 12:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 13:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 14:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 15:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
	      }
	   }
	   break;
     }
     head = head->next;
  }
}

void print_list_formatted_num(struct inst* head) {
  while(head != NULL) {
     // print the instruction number
     unsigned char ins_num = head->ins_num;
     printf("%u ", ins_num);
     // TO DO 
     unsigned char opcode = head->opcode;
     unsigned char t1 = head->t1;
     unsigned char v1 = head->v1;
     unsigned char t2 = head->t2;
     unsigned char v2 = head->v2;

     switch (opcode)
     {
	case 0: // MOV A B
	   printf("    MOV ");
	   // t1 could be REG, STK PTR
	   switch (t1)
	   {
	      case 1: // REG
		 printf("REG %u ", v1);
		 break;
	      case 2: // STK
		 printf("STK %c ", v1 + 65);
		 break;
	      case 3: // PTR
		 printf("PTR %c ", v1 + 65);
		 break;
	      default: 
		 printf("incorrect type read in MOV t1\n");
		 break;
	   }
	   // t2 could be VAL, REG, STK, PTR
	   switch (t2) 
	   {
	      case 0: // VAL
		 printf("VAL %u\n", v2);
		 break;
	      case 1: // REG
		 printf("REG %u\n", v2);
		 break;
	      case 2: // STK
		 printf("STK %c\n", v2 + 65);
		 break;
	      case 3: // PTR
		 printf("PTR %c\n", v2 + 65);
		 break;
	      default: 
		 printf("incorrect type read in MOV t2\n");
		 break;
	   }

	   break;
	case 1: // CAL A
	   printf("    CAL ");
	   switch (t1)
	   {
	      case 0: // VAL
		 printf("VAL %u\n", v1);
		 break;
	      default: // should never enter here
		 printf("incorrect type read in\n");
		 break;
	   }
	   break;
	case 2: // RET
	   printf("    RET\n");
	   break;
	case 3: // REF A B 
	   printf("    REF ");
	   switch (t1) // REG, STK, PTR
	   {
	      case 1:
		 printf("REG %u ", v1);
		 break;
	      case 2:
		 printf("STK %c ", v1 + 65);
		 break;
	      case 3:
		 printf("REG %c ", v1 + 65);
		 break;
	      default:
		 printf("REF t1 invalid binary\n");
		 break;
	   }
	   switch (t2) // STK, PTR
	   {
	      case 2:
		 printf("STK %c\n", v2 + 65);
		 break;
	      case 3:
		 printf("PTR %c\n", v2 + 65);
		 break;
	      default:
		 printf("REF t2 invalid binary\n");
		 break;
	   }
	   break;
	case 4: // ADD A B 
	   printf("    ADD ");
	   switch (t1) // REG
	   {
	      case 1:
		 printf("REG %u ",v1);
		 break;
	      default:
		 printf("ADD t1 invalid binary\n");
		 break;
	   }
	   switch (t2) // REG
	   {
	      case 1:
		 printf("REG %u\n",v2);
		 break;
	      default:
		 printf("ADD t2 invalid binary\n");
		 break;
	   }
	   break;
	case 5: // PRINT A
	   printf("    PRINT ");
	   switch (t1) // VAL, REG, STK, PTR
	   {
	      case 0:
		 printf("VAL %u\n", v1);
		 break;
	      case 1:
		 printf("REG %u\n", v1);
		 break;
	      case 2:
		 printf("STK %c\n", v1 + 65);
		 break;
	      case 3:
		 printf("PTR %c\n", v1 + 65);
		 break;
	   }
	   break;
	case 6: // NOT A
	   printf("    NOT ");
	   switch (t1)
	   {
	      case 1: 
		 printf("REG %u\n", v1);
		 break;
	      default: 
		 printf("error in NOT t1 binary\n");
		 break;
	   }
	   break;
	case 7: // EQU A

	   printf("    EQU ");
	   switch (t1)
	   {
	      case 1: 
		 printf("REG %u\n", v1);
		 break;
	      default: 
		 printf("error in NOT t1 binary\n");
		 break;
	   }
	   break;
	default: // functions and return addresses
	   if (opcode == 16) {
	      // it's a return address
	   } else {
	      // it's a function label
	      switch (opcode)
	      {
		 case 8:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 9:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 10:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 11:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 12:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 13:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 14:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 case 15:
		    printf("FUNC LABEL %u\n", opcode - 8);
		    break;
		 default:
		    printf("Entered default case in print_formatted_num\n");
		    break;

	      }
	   }
	   break;
     }
     head = head->next;
  }
}

void init_indices(struct inst* head) {
   unsigned char i = 0;
   while (head != NULL) {
      head->ins_num = i;
      i++;
      head = head->next;
   }
}

void init_function_table(struct inst* head, unsigned char* ft, int size) {
   // firstly go through the function table and initialise all the values to max_byte_size
   for (int i = 0; i < size; i++) {
      ft[i] = MAX_BYTE_SIZE;
   }
   // go through from 0 to 7. These should correspond to your functions. 
      for (int i = 0; i < size; i++ ) {
	struct inst* temp = head;
	switch (i) 
	{
	   case 0:
	      while (temp != NULL ) {
		 if (temp->opcode == 8) {
		    ft[0] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 1:
	      while (temp != NULL ) {
		 if (temp->opcode == 9) {
		    ft[1] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 2:
	      while (temp != NULL ) {
		 if (temp->opcode == 10) {
		    ft[2] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 3:
	      while (temp != NULL ) {
		 if (temp->opcode == 11) {
		    ft[3] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 4:
	      while (temp != NULL ) {
		 if (temp->opcode == 12) {
		    ft[4] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 5:
	      while (temp != NULL ) {
		 if (temp->opcode == 13) {
		    ft[5] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 6:
	      while (temp != NULL ) {
		 if (temp->opcode == 14) {
		    ft[6] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;
	   case 7:
	      while (temp != NULL ) {
		 if (temp->opcode == 15) {
		    ft[7] = temp->ins_num;
		    break;
		 }
		 temp = temp->next;
	      }
	      break;

	   default:
	      printf("Entered default case in initialising the function table\n");
	      break;

	}
   }
}
void print_function_table(unsigned char* ft, int size) {
   printf("function number\t\tinstruction number\n");
   for (int i = 0; i < size; i++) {
      printf("\tf%d\t\t\t%u\n", i, ft[i]);
   }
}

void clear_mapping(struct mapping* map) {
   for (int i = 0; i < 32; i++) {
      (map->m0)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m1)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m2)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m3)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m4)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m5)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m6)[i] = MAX_BYTE_SIZE;
   }
   for (int i = 0; i < 32; i++) {
      (map->m7)[i] = MAX_BYTE_SIZE;
   }
}

void create_mapping(struct mapping** map) {
   if (*map == NULL) {
      *map = (struct mapping*)malloc(sizeof(struct mapping));
      clear_mapping(*map);
   } else {
      printf("map has value which may be malloced. will not malloc more\n");
   }
}

void print_map(struct mapping* map) {
   // print out m0
   printf("m0:\n");
   printf("offset/symbol: \t");
   int i0 = 0;
   for (; i0 < 31; i0++) {
      printf("%d ", i0);
   }
   printf("%d\n", i0);
   int j0 = 0;
   printf("symbol read in: ");
   for (; j0 < 31; j0++) {
      if (j0 < 10) {
	 printf("%c ", (map->m0)[j0] + 65);
      } else {
	 printf(" %c ", (map->m0)[j0] + 65);
      }
   }
   printf(" %c\n", (map->m0)[j0] + 65);
   
   // print out m1
   printf("m1:\n");
   printf("offset/symbol: \t");
   int i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   int j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m1)[j] + 65);
      } else {
	 printf(" %c ", (map->m1)[j] + 65);
      }
   }
   printf(" %c\n", (map->m0)[j] + 65);

   // print out m2
   printf("m2\n");
   printf("offset/symbol: \t");
   i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m2)[j] + 65);
      } else {
	 printf(" %c ", (map->m2)[j] + 65);
      }
   }
   printf(" %c\n", (map->m1)[j] + 65);

   // print out m3
   printf("m3\n");
   printf("offset/symbol: \t");
   i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m3)[j] + 65);
      } else {
	 printf(" %c ", (map->m3)[j] + 65);
      }
   }
   printf(" %c\n", (map->m3)[j] + 65);


   // print out m4
   printf("m4\n");
   printf("offset/symbol: \t");
   i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m4)[j] + 65);
      } else {
	 printf(" %c ", (map->m4)[j] + 65);
      }
   }
   printf(" %c\n", (map->m4)[j] + 65);

   // print out m5
   printf("m5\n");
   printf("offset/symbol: \t");
   i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m5)[j] + 65);
      } else {
	 printf(" %c ", (map->m5)[j] + 65);
      }
   }
   printf(" %c\n", (map->m5)[j] + 65);

   // print out m6
   printf("m6\n");
   printf("offset/symbol: \t");
   i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m6)[j] + 65);
      } else {
	 printf(" %c ", (map->m6)[j] + 65);
      }
   }
   printf(" %c\n", (map->m6)[j] + 65);

   // print out m7
   printf("m7\n");
   printf("offset/symbol: \t");
   i = 0;
   for (; i < 31; i++) {
      printf("%d ", i);
   }
   printf("%d\n", i);
   j = 0;
   printf("symbol read in: ");
   for (; j < 31; j++) {
      if (j < 10) {
	 printf("%c ", (map->m7)[j] + 65);
      } else {
	 printf(" %c ", (map->m7)[j] + 65);
      }
   }
   printf(" %c\n", (map->m7)[j] + 65);

}

// returns 1 if symbol exists, 0 if symbol doesn't exist in the function map
int found_in_map(unsigned char v1, struct mapping* map, unsigned char f_num) {
   switch (f_num)
   {
      case 0:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m0)[i] == v1) {
	       return 1;
	    }
	 }

	 break;
      case 1:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m1)[i] == v1) {
	       return 1;
	    }
	 }

	 break;
      case 2:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m2)[i] == v1) {
	       return 1;
	    }
	 }

	 break;
      case 3:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m3)[i] == v1) {
	       return 1;
	    }
	 }

	 break;
      case 4:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m4)[i] == v1) {
	       return 1;
	    }
	 }

	 break;
      case 5:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m5)[i] == v1) {
	       return 1;
	    }
	 }

	 break;

      case 6:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m6)[i] == v1) {
	       return 1;
	    }
	 }

	 break;
      case 7:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m7)[i] == v1) {
	       return 1;
	    }
	 }

	 break;

      default:
	 printf("Entered default case in find_in_map\n");
	 break;

   }
   // assume it doesn't exist
   return 0;
}

// replaces first instance of 255 with v1
void place_in_map(unsigned char v1, struct mapping* map, unsigned char f_num) {
   switch (f_num)
   {
      case 0:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m0)[i] == 255) {
	       (map->m0)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 1:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m1)[i] == 255) {
	       (map->m1)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 2:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m2)[i] == 255) {
	       (map->m2)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 3:
	 
	 for (int i = 0; i < 32; i++) {
	    if ((map->m3)[i] == 255) {
	       (map->m3)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 4:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m4)[i] == 255) {
	       (map->m4)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 5:
	 
	 for (int i = 0; i < 32; i++) {
	    if ((map->m5)[i] == 255) {
	       (map->m5)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 6:

	 for (int i = 0; i < 32; i++) {
	    if ((map->m6)[i] == 255) {
	       (map->m6)[i] = v1;
	       break;
	    }
	 }

	 break;
      case 7:
	 
	 for (int i = 0; i < 32; i++) {
	    if ((map->m7)[i] == 255) {
	       (map->m7)[i] = v1;
	       break;
	    }
	 }

	 break;
      default:
	 printf("Entered default case in place_in_map\n");
	 break;

   }
}



void find_function(struct inst** temp, struct inst* head, unsigned char ins_num) {
   *temp = head;
   while((*temp)->next != NULL) {
      if ((*temp)->ins_num == ins_num) {
	 return;
      }
      *temp = (*temp)->next;
   }
}

// initialises the mapping
void init_map(struct inst* head, unsigned char* ft, struct mapping* map) {
   if (head == NULL) {
      return;
   }
   // go through the function table and see where the values don't equal to MAX_BYTE_SIZE
   for (int i = 0; i < 8; i++) {
      // if they equal then you want to just continue
      if (ft[i] == MAX_BYTE_SIZE) {
	 continue;
      }
      // they don't so the function is actually present. we find the location of the function
      struct inst* cur = head;
      find_function(&cur, head, ft[i]);

      unsigned char opcode = MAX_BYTE_SIZE;
      unsigned char t1 = MAX_BYTE_SIZE;
      unsigned char v1 = MAX_BYTE_SIZE;
      while(cur->next != NULL) {
	 // check the current instruction. first opcode, then type, then value
	 opcode = cur->opcode;
	 t1 = cur->t1;
	 v1 = cur->v1;

	 // If we have MOV & STK or REF & STK
	 if ( (opcode == 0 && t1 == 2) || (opcode ==3 && t1 == 2) ) {
	    if(found_in_map(v1, map, i) == 0) {
	       place_in_map(v1, map, i);
	    }
	 }
	 
	 cur = cur->next;
      }
      // do a final check for cur
      opcode = cur->opcode;
      t1 = cur->t1;
      v1 = cur->v1;
      // If we have MOV & STK or REF & STK
      if ( (opcode == 0 && t1 == 2) || (opcode ==3 && t1 == 2) ) {
	 if(found_in_map(v1, map, i) == 0) {
	    place_in_map(v1, map, i);
	 }
      }

   }
   return;
}


// given a function number and the current map, find the index corresponding to the symbol
int find_address(unsigned char v,  struct mapping * map, unsigned char f_num) {
   if (f_num == 0) {
      for (int i = 0; i < 32; i++) {
	 if ( (map->m0)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 1) {
      for (int i = 0; i < 32; i++) {
	 if ( (map->m1)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 2) {

      for (int i = 0; i < 32; i++) {
	 if ( (map->m2)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 3) {

      for (int i = 0; i < 32; i++) {
	 if ( (map->m3)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 4) {

      for (int i = 0; i < 32; i++) {
	 if ( (map->m4)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 5) {

      for (int i = 0; i < 32; i++) {
	 if ( (map->m5)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 6) {

      for (int i = 0; i < 32; i++) {
	 if ( (map->m6)[i] == v) {
	    return i;
	 }
      }

   } else if (f_num == 7) {

      for (int i = 0; i < 32; i++) {
	 if ( (map->m7)[i] == v) {
	    return i;
	 }
      }

   } else {
      printf("invalid function entered in find_address function. returning -1\n");
   }
   return -1;
}


// inputting the current map. update as you go
void print_single_formatted_with_mapping(struct inst* head, struct mapping* map, unsigned char func_num) {
   if (head == NULL) {
      return;
   }
   unsigned char opcode = head->opcode;
   unsigned char t1 = head->t1;
   unsigned char v1 = head->v1;
   unsigned char t2 = head->t2;
   unsigned char v2 = head->v2;
   if (opcode == 0) { // MOV A (REG, STK, PTR) B(VAL, REG, STK, PTR)
      printf("    MOV ");
      if (t1 == 1) {
	 printf("REG %u ",v1); 
      } else if (t1 == 2) {
	 // STK if value doesn't exist, increment stack pointer then add the value to the stack. Also place the value into the map. 
	 int address = find_address(v1, map, func_num);
	 if (address == -1 ) {
	    // add it to the map
	    // find the address
	    place_in_map(v1, map, func_num); 
	    address = find_address(v1, map, func_num);
	    printf("STK %c ", address + 65);
	 } else {
	    // it's in the map so just print the address
	    printf("STK %c ", address + 65);
	 }
      } else if (t1 == 3) {
	 // PTR 
	 int address = find_address(v1, map, func_num);
	 if (address == -1) {
	    printf("error ptr symbol not found\n");
	    return;
	 }
	 printf("PTR %c ", address + 65);
      } else {
	 printf("ERROR IN MOVE as type is wrong\n");
	 return;
      }
      
      if (t2 == 0) {
	 printf("VAL %u\n", v2);
      } else if (t2 == 1) {
	 printf("REG %u\n", v2);
      } else if (t2 == 2) {
	 // STK. 
	 int address = find_address(v2, map, func_num);
	 if (address == -1 ){
	    printf("error stk symbol not found in mov t2\n");
	    return;
	 }
	 printf("STK %c\n", address + 65);
      } else if (t2 == 3) {
	 // PTR
	 int address = find_address(v2, map, func_num);
	 if (address == -1 ){
	    printf("error stk symbol not found in mov t2\n");
	    return;
	 }
	 printf("PTR %c\n", address + 65);

      }
   } else if (opcode == 1) { // CAL A (VAL)
      printf("    CAL ");
      // check type
      if (t1 != 0) {
	 printf("Error in CAL VAL RRINT AS VAL ISNT THE TYPE\n");
	 return;
      }
      printf("VAL ");
      printf("%u\n", v1);

   } else if (opcode == 2) { // RET
      printf("    RET\n");
   } else if (opcode == 3) { // REF A (REG, STK, PTR), B (STK, PTR)
      printf("    REF ");
      if (t1 == 1) {
	 printf("REG %u ", v1);
      } else if (t1 == 2) {

	 // STK  
	 int address = find_address(v1, map, func_num);
	 if (address == -1 ) {

	    place_in_map(v1, map, func_num); 
	    address = find_address(v1, map, func_num);
	    printf("STK %c ", address + 65);
	 } else {
	    printf("STK %c ", address + 65);
	 }
 
      } else if (t1 == 3) {
	 // PTR 
	 int address = find_address(v1, map, func_num);
	 if (address == -1) {
	    printf("error ptr symbol not found\n");
	    return;
	 }
	 printf("PTR %c ", address + 65);

      } else {
	 printf("ERROR IN REF AS TYPE IS WRONG for t1\n");
	 return;
      }

      if (t2 == 2) {
	 // STK. 
	 int address = find_address(v2, map, func_num);
	 if (address == -1 ){
	    printf("error stk symbol not found in mov t2\n");
	    return;
	 }
	 printf("STK %c\n", address + 65);

      } else if (t2 == 3) {
	 // PTR
	 int address = find_address(v2, map, func_num);
	 if (address == -1 ){
	    printf("error stk symbol not found in mov t2\n");
	    return;
	 }
	 printf("PTR %c\n", address + 65);

      } else {
	 printf("Error in REF with t2\n");
	 return;
      }
   } else if (opcode == 4) { // ADD A (REG) B (REG)
      printf("    ADD ");
      // check type
      if (t1 != 1) {
	 printf("ERROR IN ADD AS t1 ISNT RIGHT\n");
	 return;
      }
      if (t2 != 1) {
	 printf("ERROR IN ADD AS t2 ISN'T THE RIGHT TYPE\n");
	 return;
      }
      printf("REG %u REG %u\n", v1, v2);
   } else if (opcode == 5) { // PRINT A (VAL, REG, STK, PTR)
      printf("    PRINT ");
      if (t1 == 0) {
	 printf("VAL %u\n", v1);
      } else if (t1 == 1) {
	 printf("REG %u\n", v1);
      } else if (t1 == 2) {
	 // STK 
	 int address = find_address(v1, map, func_num);
	 if (address == -1 ) {
	    printf("error in print as symbol not found\n");
	 } else {
	    printf("STK %c\n", address + 65);
	 }

      } else if (t1 == 3) {
	 // PTR 
	 int address = find_address(v1, map, func_num);
	 if (address == -1) {
	    printf("error ptr symbol not found in print\n");
	    return;
	 }
	 printf("PTR %c\n", address + 65);

      } else {
	 printf("ERROR IN PRINT AS TYPE ISN'T CORRECT\n");
      }
   } else if (opcode == 6) { // NOT A (REG)
      if (t1 != 1) {
	 printf("ERROR IN NOT AS TYPE ISN'T CORRECT\n");
	 return;
      }
      printf("    NOT REG %u\n", v1);
   } else if (opcode == 7) { // EQU A (REG)
      if (t1 != 1) {
	 printf("ERROR IN EQU AS TYPE ISN'T CORRENT\n");
	 return;
      }
      printf("    EQU REG %u\n", v1);
   } else { // opcode >= 8. don't forget to -8
      printf("other if statement shouldve taken care of this\n");
   }
}
// inputting a cleared map. update as you go
void print_list_formatted_with_mapping(struct inst* head, struct mapping* map) {
   if (head == NULL || map == NULL) {
      return;
   }

   unsigned char current_function = 255;

   while(head->next != NULL ) {
   unsigned char opcode = head->opcode;
   if (opcode > 7) {
      if (opcode == 8) {
	 current_function = 0;
	 printf("FUNC LABEL %u\n", opcode - 8);
      } else if (opcode == 9) {
	 current_function = 1;
	 printf("FUNC LABEL %u\n", opcode - 8);

      } else if (opcode == 10) {
	 current_function = 2;
	 printf("FUNC LABEL %u\n", opcode - 8);

      } else if (opcode == 11) {
	 current_function = 3;
	 printf("FUNC LABEL %u\n", opcode - 8);

      } else if (opcode == 12) {
	 current_function = 4;
	 printf("FUNC LABEL %u\n", opcode - 8);

      } else if (opcode == 13) {
	 current_function = 5;
	 printf("FUNC LABEL %u\n", opcode - 8);

      } else if (opcode == 14) {
	 current_function = 6;
	 printf("FUNC LABEL %u\n", opcode - 8);

      } else if (opcode == 15) {
	 current_function = 7;
	 printf("FUNC LABEL %u\n", opcode - 8);
      }
   } else {
      print_single_formatted_with_mapping(head, map, current_function);
   }
      head = head->next;
   }
   print_single_formatted_with_mapping(head, map, current_function);
}

void find_function_number(unsigned char ins_num, struct inst* head, unsigned char* f_num) {
   unsigned char opcode = head->opcode; // points to some function label
   struct inst* temp = head;
   while (temp->next != NULL) {

      if (temp->ins_num == ins_num) {
	 break;
      }
      if(temp->opcode > 7 && temp->opcode < 16) {
	 opcode = temp->opcode;
      }
      temp = temp->next;
   }
   *f_num = opcode - 8;

}

void find_f0_ret(struct inst* head, unsigned char* ret) {
  // go to label 0 
  struct inst* temp = head;
  while (temp->next != NULL) {
     if (temp->opcode == 8) {
	break;
     }
     temp = temp->next;
  }
  while (temp->next != NULL ) {
     if (temp->opcode == 2) {
	break;
     }
     temp = temp->next;
  }
  if (temp->opcode == 2) {
     *ret = temp->ins_num;
     return;
  }
  *ret = 0;
}

// executes a single command. the single command is given by a pointer. need to know the current function you're working in 
// returns 0 on success and 1 on failiure
int execute(struct inst* command, struct mapping* map, unsigned char* stack, unsigned char* registers, unsigned char f_num) {
   if (command == NULL || map == NULL) {
      return 1;
   }

      // fill in here
   unsigned char opcode = command->opcode;
   unsigned char t1 = command->t1;
   unsigned char v1 = command->v1;
   unsigned char t2 = command->t2;
   unsigned char v2 = command->v2;
   unsigned char bp = registers[5];
   unsigned char sp = registers[6];
   if (opcode == 0) { // MOV A (REG, STK, PTR) B(VAL, REG, STK, PTR)

      // MOV REG ...
      if (t1 == 1 && t2 == 0) {	 // MOV REG VAL
	 registers[v1] = v2;
      } else if (t1 == 1 && t2 == 1) { // MOV REG REG
	 registers[v1] = registers[v2];
      } else if (t1 == 1 && t2 == 2) { // MOV REG STK
	 int address = find_address(v2, map, f_num);
	 if (address == -1) {
	    printf("error in MOV REG STK. v2 doesn't exist\n");
	    return 1;
	 }
	 registers[v1] = stack[bp + address];
      } else if (t1 == 1 && t2 == 3) { // MOV REG PTR
	 int address = find_address(v2, map, f_num);
	 if (address == -1) {
	    printf("error in MOV REG PTR. v2 doesn't exist\n");
	    return 1;
	 }
	 unsigned char apparent_address = stack[bp + address];
	 registers[v1] = stack[apparent_address];
      }

      // MOV STK ... 
      // could potentially lead to a new stk variable. 2 cases each one. if symbol exists or if symbol doesn't exist. if it doesn't exist, place it in the map, 
      // place the value at the new location and increment the stk pointer accordingly. 
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      else if (t1 == 2 && t2 == 0) { // MOV STK VAL

	 int check_address = find_address(v1, map, f_num);	     // check address is in map
	 if (check_address == -1) {				     // if address isn't in map
	    place_in_map(v1, map, f_num);			     // place it in the map and make a new stack symbol on the stack 
	    registers[6] = sp + 1;				     // increment the stack pointer
	 }
	 stack[bp + find_address(v1, map, f_num)] = v2;		     // place in the stack

      /////////////////////////////////////////////////////// THERE MIGHT BE ANOTHER CASE FOR WHEN YOU HAVE ANOTHER FRAME //////////////////////////////////////
      } else if (t1 == 2 && t2 == 1) { // MOV STK REG

	 int check_address = find_address(v1, map, f_num);
	 if (check_address == -1) {
	    place_in_map(v1, map, f_num);
	    registers[6] = sp + 1;
	 }
	 stack[bp + find_address(v1, map, f_num)] = registers[v2];

      } else if (t1 == 2 && t2 == 2) { // MOV STK STK

	 int check_address = find_address(v1, map, f_num);
	 if (check_address == -1) {
	    place_in_map(v1, map, f_num);
	    registers[6] = sp + 1;
	 }
	 stack[bp + find_address(v1, map, f_num)] = stack[find_address(v2, map, f_num)];

      } else if (t1 == 2 && t2 == 3) { // MOV STK PTR

	 int check_address = find_address(v1, map, f_num);
	 if (check_address == -1) {
	    place_in_map(v1, map, f_num);
	    registers[6] = sp + 1;
	 }
	 unsigned char apparent_address = stack[bp + find_address(v2, map ,f_num)];    // find thr value but then treat is as an address whose value want
	 stack[bp + find_address(v1, map, f_num)] = stack[apparent_address];

      }

      // MOV PTR ...
      // we're assuming this will already exist
      else if (t1 == 3 && t2 == 0) { // MOV PTR VAL

	 int address = find_address(v1, map, f_num);
	 if (address == -1) {
	    printf("Symbol v1 not found in MOV PTR VAL...\n");
	    return 1;
	 }
	 unsigned char apparent_address = stack[bp + address];
	 stack[apparent_address] = v2;

      } else if (t1 == 3 && t2 == 1) { // MOV PTR REG

	 int address = find_address(v1, map, f_num);
	 if (address == -1) {
	    printf("Symbol v1 not found in MOV PTR REG...\n");
	    return 1;
	 }
	 unsigned char apparent_address = stack[bp + address];
	 stack[apparent_address] = registers[v2];

      } else if (t1 == 3 && t2 == 2) { // MOV PTR STK

	 int address = find_address(v1, map, f_num);
	 if (address == -1) {
	    printf("Symbol v1 not found in MOV PTR STK...\n");
	    return 1;
	 }
	 unsigned char apparent_address = stack[bp + address];

	 address = find_address(v2, map, f_num); // resuse the var address
	 if (address == -1) {
	    printf("Symbol for v2 not found in MOV PTR STK...\n");
	    return 1;
	 }

	 stack[apparent_address] = stack[address];

      } else if (t1 == 3 && t2 == 3) { // MOV PTR PTR
	 int address = find_address(v1, map, f_num);
	 if (address == -1) {
	    printf("symbol v1 not found in MOV PTR PTR\n");
	    return 1;
	 }
	 unsigned char apparent_address1 = stack[bp + address];

	 address = find_address(v2, map, f_num);
	 if (address == -1) {
	    printf("symbol v2 not found in MOV PTR PTR\n");
	    return 1;
	 }
	 unsigned char apparent_address2 = stack[bp + address];
	 stack[apparent_address1] = stack[apparent_address2];
      } else {
	 printf("Not a valid combination in MOV\n");
	 return 1;
      }

   } else if (opcode == 1) { // CAL A (VAL)
            
   } else if (opcode == 2) { // RET

      
   } else if (opcode == 3) { // REF A (REG, STK, PTR), B (STK, PTR)

      if (t1 == 1 && t2 == 2) { // REF REG STK
	 int address = find_address(v2, map, f_num);
	 if (address == -1) {
	    printf("error in REF REG STK v2\n");
	    return 1;
	 }
	 registers[v1] = bp + address;
      } else if (t1 == 1 && t2 == 3) { // REF REG PTR <==> MOV REG STK
	 int address = find_address(v2, map, f_num);
	 if (address == -1) {
	    printf("error in REF REG PTR v2\n");
	    return 1;
	 }
	 registers[v1] = stack[bp + address];
      }

      else if (t1 == 2 && t2 == 2) { // REF STK STK
	 int address2 = find_address(v2, map, f_num);
	 if (address2 == -1) {
	    printf("error in REF STK STK v2\n");
	    return 1;
	 }
	 int address1 = find_address(v1, map, f_num);
	 if (address1 == -1) {
	    place_in_map(v1, map, f_num);
	    registers[5] = sp + 1;
	    address1 = find_address(v1, map, f_num);
	 }
	 stack[bp + address1] = address2 + bp;
      } else if (t1 == 2 && t2 == 3) { // REF STK PTR <==> MOV STK STK
	 int address2 = find_address(v2, map, f_num);
	 if (address2 == -1) {
	    printf("error in REF STK PTR v2\n");
	    return 1;
	 }

	 int address1 = find_address(v1, map, f_num);
	 if (address1 == -1) {
	    place_in_map(v1, map, f_num);
	    registers[5] = sp + 1;
	    address1 = find_address(v1, map, f_num);
	 }
	 stack[bp + address1] = stack[bp + address2];
      }
      
      else if (t1 == 3 && t2 == 2) { // REF PTR STK
	 int address2 = find_address(v2, map, f_num);
	 if (address2 == -1) {
	    printf("error in REF PTR STK v2\n");
	    return 1;
	 }
	 int address1 = find_address(v1, map, f_num);
	 if (address1 == -1) {
	    printf("error in REF PTR STK v1\n");
	    return 1;
	 }
	 unsigned char apparent_address1 = stack[bp + address1];
	 stack[apparent_address1] = bp + address2;
      } else if (t1 == 3 && t2 == 3) { // REF PTR PTR <==> MOV PTR STK
	 int address2 = find_address(v2, map, f_num);
	 if (address2 == -1) {
	    printf("error in REF PTR PTR v2\n");
	    return 1;
	 }
	 int address1 = find_address(v1, map, f_num);
	 if (address1 == -1) {
	    printf("error in REF PTR PTR v1\n");
	    return 1;
	 }
	 unsigned char apparent_address1 = stack[bp + address1];
	 stack[apparent_address1] = stack[bp + address2];
      }

      else {
	 printf("Not a valid combination in REF\n");
	 return 1;
      }
    

   } else if (opcode == 4) { // ADD A (REG) B (REG)
      
      if (t1 == 1 && t2 == 1) { // ADD REG REG
	 registers[v1] = registers[v1] + registers[v2];
      }
      
      else {
	 printf("Not a valid combination in ADD\n");
	 return 1;
      }

   } else if (opcode == 5) { // PRINT A (VAL, REG, STK, PTR)
      
      if (t1 == 0) {
	 printf("%u\n", v1);
      } else if (t1 == 1) {
	 printf("%u\n", registers[v1]);
      } else if (t1 == 2) {
	 int address = find_address(v1, map, f_num);
	 if (address == -1) {
	    printf("Error in PRINT STK v1\n");
	    return 1;
	 }
	 printf("%u\n", stack[bp + address]);
      } else if (t1 == 3){
	 int address = find_address(v1, map, f_num);
	 if (address == -1) {
	    printf("Error in Print ptr v1\n");
	    return 1;
	 }
	 unsigned char apparent_address = stack[bp + address];
	 printf("%u\n", stack[apparent_address]);
      } else {
	 printf("error in PRINT\n");
	 return 1;
      }

   } else if (opcode == 6) { // NOT A (REG)
      if (t1 == 1) {
	 registers[v1] = ~registers[v1];
      } else {
	 printf("error in NOT\n");
	 return 1;
      }

   } else if (opcode == 7) { // EQU A (REG)
      if (t1 == 1) {
	 unsigned char value = registers[v1];
	 if (value == 0) {
	    registers[v1] = 1;
	 } else {
	    registers[v1] = 0;
	 }
      } else {
	 printf("error in EQU\n");
	 return 1;
      }

   } else { // an opcode >= 8 is a function label. don't do anything, just return success
      if (opcode > 15) {
	 printf("bad function\n");
	 return 1;
      }
   }
      return 0;
} 
