#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/* Makes the the path to the read fifo. I.e <domain>/<identifier>_RD*/
void make_read_path(char* path, char* domain, char* identifier);

/* Makes the the path to the write fifo. I.e <domain>/<identifier>_WR*/
void make_write_path(char* path, char* domain, char* identifier);

/* returns 1 upon entering DISCONNECT and 0 elsewhere */
int client_handler(char* domain, char* identifier, char* identifier_wr, char* identifier_rd, int id2);

/* returns 1 if it contains the string "_RD" at the end else 0 */
int is_rd_fifo(char* str);

#endif
