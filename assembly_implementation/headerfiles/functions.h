#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_BYTE_SIZE 255
#define GO 1
#define STOP 0

/* The point of this header files is to expose the functino protoypes used when converting the binary file into an array used by the program*/

// 1. reads the binary from file_name into file_buffer and stores the amount ofbytes in address_size_file
void read_into_buffer(const char * file_name, unsigned char** file_buffer, int* address_size_file);

// 2. shifts buffer by n bytes to the right
void shift_by_byte(unsigned char* file_buffer, int buffer_size, int n);

// 3. shift right by n (between 1 and 7) with rotation
void rotate_right(unsigned char* file_buffer, int n);

// 4. returns the quotient when dividing by 8
int quotient_by_8(int n);

// 5. returns the remainder when dividing by 8
int remainder_by_8(int n);

// 6. shifts the whole array by n bits to the right 
void shift_right_bitwise(unsigned char* file_buffer, int buffer_size, int n);

// 7. 
unsigned char read_and_shift_right(unsigned char* file_buffer, int buffer_size, int n);

// 8.
void read_binary(unsigned char* file_buffer, int buffer_size);
#endif
