#include <stdio.h>
#include <stdlib.h>
#include "../headerfiles/functions.h"

// 1. 
void read_into_buffer(const char * file_name, unsigned char** file_buffer, int* address_size_file) {
   // We read from file into our file buffer
   
   FILE * fp = fopen(file_name, "rb");
   if (fp == NULL) {
      fprintf(stderr, "error:file not found\n");
      return;
   }
   
   fseek(fp, 0, SEEK_END);
   *address_size_file = ftell(fp);
   fseek(fp, 0, SEEK_SET);
   
   *file_buffer = (unsigned char*)malloc(*address_size_file);
   
   if (*file_buffer == NULL) {
      fprintf(stderr, "error: couldn't allocate enough memory\n");
      return;
   }
   
   int num_read_in = fread(*file_buffer, 1, *address_size_file, fp);
   
   if (num_read_in != *address_size_file) {
      fprintf(stderr, "error: couldn't read in all bytes\n");
      return;
   }
   fclose(fp);

}


// 2. 
void shift_by_byte(unsigned char* file_buffer, int buffer_size, int n) {
   int j = 0;
   while (j < n) {
      int i = buffer_size - 1;
      for (; i > 0; i--) {
	 file_buffer[i] = file_buffer[i - 1];
      }
      file_buffer[i] = 0;
      j++;
   }
}

// 3. 
void rotate_right(unsigned char* file_buffer, int n) {
  unsigned char a = *file_buffer >> n; 
  unsigned char b = *file_buffer << (8 - n);
  *file_buffer = a | b;
}

// 4.
int quotient_by_8(int n) {
   int quotient = 0;
   while (n >= 8) {
      n = n - 8;
      quotient++;
   }
   return quotient;
}

// 5.
int remainder_by_8(int n) {
   int remainder = 0;

   while (n >= 8) {
      n = n - 8;
   }
   remainder = n;
   return remainder;
}

// 6. 
void shift_right_bitwise(unsigned char* file_buffer, int buffer_size, int n) {
   // deal with the quotients first
   int quotient = quotient_by_8(n);
   int remainder = remainder_by_8(n);
   shift_by_byte(file_buffer, buffer_size, quotient); 

   // deal with remainder: shift every 
   int i = buffer_size - 1;
   unsigned char result = 0;
   switch (remainder) 
   {
      case 1:
	 
	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 1;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x01;
	    rotate_right( &temp2 , 1);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 1;

	 break;
      case 2:
	 
	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 2;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x03;
	    rotate_right(&temp2, 2);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 2;

	 break;
      case 3:

	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 3;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x07;
	    rotate_right(&temp2, 3);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 3;

	 break;
      case 4:

	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 4;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x0F;
	    rotate_right(&temp2, 4);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 4;

	 break;
      case 5:
	 
	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 5;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x1F;
	    rotate_right(&temp2, 5);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 5;

	 break;
      case 6:

	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 6;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x3F;
	    rotate_right(&temp2, 6);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 6;

	 break;
      case 7:

	 while (i > 0) {
	    unsigned char temp1 = file_buffer[i];
	    temp1 = temp1 >> 7;
	    unsigned char temp2 = file_buffer[i - 1];
	    temp2 = temp2 & 0x7F;
	    rotate_right(&temp2, 7);
	    result = temp1 | temp2;
	    file_buffer[i] = result;
	    i--;
	 }
	 file_buffer[i] = file_buffer[i] >> 7;

	 break;
      default:
	 break;
   }
}

// 7. we're always reading from the last byte. n ranges from 0 to 8 and. If n is 0, throw an error 
unsigned char read_and_shift_right(unsigned char* file_buffer, int buffer_size, int n) {
   
   // go to the last byte and copy it
   unsigned char temp = file_buffer[buffer_size -1];

   if (n == 0) {
      printf("error, you have to read in 1-8 bits\n");
   } else{

     switch(n) 
     {
	// read in 1 bit 0000 0001
	case 1 :
	   temp = temp & 0x01;
	   shift_right_bitwise(file_buffer, buffer_size, 1);
	   break;

	case 2 :
	   temp = temp & 0x03;
	   shift_right_bitwise(file_buffer, buffer_size, 2);
	   break;

	case 3 :
	   temp = temp & 0x07;
	   shift_right_bitwise(file_buffer, buffer_size, 3);
	   break;

	case 4 :
	   temp = temp & 0x0F;
	   shift_right_bitwise(file_buffer, buffer_size, 4);
	   break;

	case 5 :
	   temp = temp & 0x1F;
	   shift_right_bitwise(file_buffer, buffer_size, 5);
	   break;

	case 6 :
	   temp = temp & 0x3F;
	   shift_right_bitwise(file_buffer, buffer_size, 6);
	   break;

	case 7 :
	   temp = temp & 0x7F;
	   shift_right_bitwise(file_buffer, buffer_size, 7);
	   break;

	// read in 8 bits (or a byte)
	case 8 :
	   shift_right_bitwise(file_buffer, buffer_size, 8);
	   break;

	default :
	   break;
     }
   }
   return temp;
}

// TO DO:  ACCOUNT FOR lower case letters in case 0 (MOV) cause I ceebsed doing it. 
void read_binary(unsigned char* file_buffer, int buffer_size) {
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
	       printf("    MOV ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       switch(t1)
	       {
		  case 1 : // t1 is a register
		     printf("REG ");
		     
		     v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		     printf("%u ", v1);
		     break;
		  case 2 : // t1 is a stack symbol
		     printf("STK ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     printf("%c ", v1 + 65);
		     break;
		  case 3: // t1 is a pointer symbol
		     printf("PTR ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     printf("%c ", v1 + 65);
		     break;
		  default: 
		     printf("default entered for MOV t1\n");
		     break;
	       }

	       t2 = read_and_shift_right(file_buffer, buffer_size, 2);
	       switch(t2)
	       {
		  case 0 : // t2 is a value
		     printf("VAL ");

		     v2 = read_and_shift_right(file_buffer, buffer_size, 8);
		     printf("%u\n", v2);
		     break;

		  case 1 : // t2 is a register
		     printf("REG ");

		     v2 = read_and_shift_right(file_buffer, buffer_size, 3);
		     printf("%u\n", v2);
		     break;
		  
		  case 2: // t2 is a stack symbol
		     printf("STK ");

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     printf("%c\n", v2 + 65);
		     break;
		  case 3: // t2 is a pointer symbol
		     printf("PTR ");

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5); // NEED IF STATEMENT FOR SMALLER LETTERS
		     printf("%c\n", v2 + 65);
		     break;
		  default: 
		     printf("default entered for MOV t2\n");
		     break;
	       }

	       break;

	    case 1 : // CAL A 
	       printf("    CAL ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t1 != 0) {
		  printf("error binary wrong\n");
	       } else {
		  printf("VAL ");

		  v1 = read_and_shift_right(file_buffer, buffer_size, 8);
		  if (v1 > 7) {
		     printf("Invalid function in binary\n");
		  } else {

		     printf("%u\n", v1);
		  }
	       }
	       break;

	    case 2 : // RET 
	       printf("    RET\n");
	       break;

	    case 3 : // REF A B: A can be pointer valued, stack valued or register valued. IT CANNOT BE A VALUE. B can only be stack or pointer valued (not reg). 
	       printf("    REF ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2); // t1 can be ptr, stack or register
	       switch(t1)
	       {
		  case 1: // register address
		     printf("REG ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		     printf("%u ", v1);
		     break;

		  case 2: // stack symbol
		     printf("STK ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     printf("%c ", v1 + 65);
		     break;

		  case 3: // pointer symbol
		     printf("PTR ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     printf("%c ", v1 + 65);
		     break;

		  default: 
		     printf("Entered default for REF t1\n");
		     break;
	       }

	       t2 = read_and_shift_right(file_buffer, buffer_size, 2); // CANNOT have register symbol. 
	       switch(t2)
	       {
		  case 2: // stack symbol
		     printf("STK ");

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5);
		     printf("%c\n", v2 + 65);
		     break;

		  case 3: // pointer symbol
		     printf("PTR ");

		     v2 = read_and_shift_right(file_buffer, buffer_size, 5);
		     printf("%c\n", v2 + 65);
		     break;

		  default: 
		     break;
	       }
	       break;

	    case 4 : // ADD A B 
	       printf("    ADD ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if ( t1 != 1) {
		  printf("error in binary for add in t1\n");
	       } else {
		  printf("REG ");

		  v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		  printf("%u ", v1);
	       }

	       t2 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t2 != 1) {
		  printf("error in binary for add in t2\n");
	       } else {
		  printf("REG ");

		  v2 = read_and_shift_right(file_buffer, buffer_size, 3);
		  printf("%u\n", v2);
	       }
	       break;

	    case 5 : // PRINT A 
	       printf("    PRINT ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       switch(t1)
	       {
		  case 0: // value
		     printf("VAL ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 8);
		     printf("%u\n", v1);
		     break;
		  case 1: // register
		     printf("REG ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		     if (v1 > 7) {
			printf("error in binary in print v1\n");
		     } else {
			printf("%u\n", v1);
		     }
		     break;

		  case 2: // stack 
		     printf("STK ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     printf("%c\n", v1 + 65);
		     break;

		  case 3:  // pointer
		     printf("PTR ");

		     v1 = read_and_shift_right(file_buffer, buffer_size, 5);
		     printf("%c\n", v1 + 65);
		     break;
	       }
	       break;

	    case 6 : // NOT A 
	       printf("    NOT ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t1 != 1) {
		  printf("error in binary in NOT t1\n");
	       } else {
		  printf("REG ");
		  v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		  printf("%u\n", v1);
	       }
	       break;

	    case 7 : // EQU A 
	       printf("    EQU ");

	       t1 = read_and_shift_right(file_buffer, buffer_size, 2);
	       if (t1 != 1) {
		  printf("error in binary in EQU t1\n");
	       } else {
		  printf("REG ");
		  v1 = read_and_shift_right(file_buffer, buffer_size, 3);
		  printf("%u\n", v1);
	       }
	       break;

	 }
      }

      // read in the next 3 bits for the function label
      unsigned char fun_lab = read_and_shift_right(file_buffer, buffer_size, 3);
      printf("FUNC LABEL %u\n", fun_lab);

      // check the next 5 bits. if they are non zero we repeat. if they are zero you stop. 
      num_ins = read_and_shift_right(file_buffer, buffer_size, 5);
      if (num_ins == 0) {
	 keep_reading = STOP; 
      }
   }
}
