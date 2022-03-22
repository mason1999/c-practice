#ifndef LIST_H
#define LIST_H

/* This header file exposes the functions used in creating the program memory and the map. The map is used when reading from program memory and storing in stack*/

#define MAX_RAM 256
struct inst {
   unsigned char opcode;
   unsigned char t1;
   unsigned char v1;
   unsigned char t2;
   unsigned char v2;
   unsigned char ins_num;
   struct inst * next;
};

// this is supposed to be the mapping for symbol in the current function to a place in RAM
struct mapping {
   unsigned char m0[32];
   unsigned char m1[32];
   unsigned char m2[32];
   unsigned char m3[32];
   unsigned char m4[32];
   unsigned char m5[32];
   unsigned char m6[32];
   unsigned char m7[32];
};

// 1. function creates an inst struct and returns a pointer to it
struct inst* create(unsigned char opcode, unsigned char t1, unsigned char v1, unsigned char t2, unsigned char v2);

// 2. function pushes to head of linked list. Need address of head because head value will change
void push(struct inst** head, unsigned char opcode, unsigned char t1, unsigned char v1, unsigned char t2, unsigned char v2);

// 3. function pops the first node
void pop(struct inst** head);

// 4. frees all the associated memory
void free_list(struct inst** head);

// 5. Prints a single instruction
void print_single(struct inst * head);

// 6. Prints a whole list of instructions
void print_list(struct inst* head);

// 7. create a linked list of instructions from file buffer
void create_program_memory(unsigned char* file_buffer, int buffer_size, struct inst** head);

// 8. prints linked list in order from head
void print_list_formatted(struct inst* head);

// 9. prints linked list in order with the number
void print_list_formatted_num(struct inst* head);

// 10. initialises the indices
void init_indices(struct inst* head);

// 11. initialises the function table
void init_function_table(struct inst* head, unsigned char* ft, int size);

// 12. Prints the function table
void print_function_table(unsigned char* ft, int size);

// 13. initalises all the values to 255 in the table
void clear_mapping(struct mapping* map);

// 14. create a mapping with cleared values (where cleared values means the value 255 here)
void create_mapping(struct mapping** map);

// 14. prints the map in a nicely formatted way
void print_map(struct mapping* map);

// 15. checks to see if a symbol (number) v1 extists in the current mapping
int found_in_map(unsigned char v1, struct mapping* map, unsigned char f_num);

// 16. places v1 at the first spot it can find, replacing 255 with it. 
void place_in_map(unsigned char v1, struct mapping* map, unsigned char f_num);

// 17. Given the instruction number to a function, we update temp to point
// to that node
void find_function(struct inst** temp, struct inst* head, unsigned char ins_num);

// 18. initalises the map that we have with the values in our program code
void init_map(struct inst* head, unsigned char* ft, struct mapping* map);

// 19. returns index/offset in stack frame for the current value. if not found, returns -1 
int find_address(unsigned char v, struct mapping* map, unsigned char f_num);

// 20. prints a single instruction with formatting
void print_single_formatted_with_mapping(struct inst* head, struct mapping* map, unsigned char func_num); 

// 21. prints list formatted with mapping
void print_list_formatted_with_mapping(struct inst* head, struct mapping* map);

// 22. finds the current function you're in and stores it in f_num which represents register 4 // 
void find_function_number(unsigned char ins_num, struct inst* head, unsigned char* f_num);

// 23. Finds the first instance of RET in f_0(will exclusively be used for function zero. Code will break otherwise) // limiitations: doesn't account for multiple 
// rets in function 0
void find_f0_ret(struct inst* head, unsigned char* ret);

// 24. Execute
int execute(struct inst* command, struct mapping* map, unsigned char* stack, unsigned char* registers, unsigned char f_num);


#endif
