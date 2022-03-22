#include "server.h"
#include "functions.h"
#include <stdio.h>
#include <stddef.h>	   
#include <string.h>
#include <dirent.h>	   
#include <errno.h>
#include <fcntl.h>	   
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>	   
#include <sys/poll.h>
#include <stdint.h>
#include <signal.h>	   


void make_read_path(char* path, char* domain, char* identifier) {

	strncpy(path, domain, MAX_NAME_LENGTH);
	strcat(path, "/");
	strcat(path, identifier);
	strcat(path, "_RD");

}

void make_write_path(char* path, char* domain, char* identifier) {

	strncpy(path, domain, MAX_NAME_LENGTH);
	strcat(path, "/");
	strcat(path, identifier);
	strcat(path, "_WR");

}

int is_rd_fifo(char* str) {

	int length = strlen(str);
	
	if (str[length - 3] == '_' && str[length - 2] == 'R' && str[length - 1] == 'D') {
		return 1;
	}

	return 0;
}

int client_handler(char* domain, char* identifier, char* identifier_wr, char* identifier_rd, int id2) {


	/* message = the 2048 byte message we read in from the clients write pipe
	* client_wr = path to client's write pipe
	* client_rd = path to client's read pipe */

	char message[PACKET_SIZE];

	while(1) {

		int read_from_write_pipe_fd = open(identifier_wr, O_RDONLY);
		read(read_from_write_pipe_fd, message, PACKET_SIZE);

		// read from the file descriptor into the message and check the type
		uint16_t* type = (uint16_t*)message;

		if (*type == DISCONNECT) {

			// unlink the fifos
			unlink(identifier_wr);
			unlink(identifier_rd);

			kill(getppid(), SIGUSR1);
			return 1;

		} else if (*type == SAY) {

			/* We form the message to be sent here in this code chunk */
			char send[PACKET_SIZE];
			memset(send, 0, PACKET_SIZE);

			// memcpy from type to send 2 bytes
			uint16_t type = RECEIVE;
			memcpy(send, &type, TYPE_SIZE);

			// memcpy from identifier to (send + 2) 256 bytes
			memcpy(send + TYPE_SIZE, identifier, MAX_NAME_LENGTH);

			// memcpy from (message + 2) to (send + 2 + 256) 1790 bytes
			memcpy(send + TYPE_SIZE + MAX_NAME_LENGTH, message + TYPE_SIZE, SAY_MAX);


			/* We now scan through the current directory. We write to all the _RD pipes that are not the client's read pipe */
			char identifier_rd[MAX_NAME_LENGTH];
			strcpy(identifier_rd, identifier);
			strcat(identifier_rd, "_RD");

			DIR* dir = opendir(domain);
			if (dir == NULL) {
				fprintf(stderr, "Error in client_handler: couldn't open domain\n");
				return 0;
			}
			struct dirent* entity;
			entity = readdir(dir);
			while (entity != NULL) {

				// If "_RD" is the last three characters of entity->d_name
				if ( is_rd_fifo(entity->d_name) ) {
					// if the current "_RD" file isn't "identifier_RD"
					if (strcmp(entity->d_name, identifier_rd) != 0) {
						// open, write and close to ALL the other clients "_RD" pipes in the same domain
						char current_entity[MAX_NAME_LENGTH];
						strcpy(current_entity, domain);
						strcat(current_entity, "/");
						strcat(current_entity, entity->d_name);
						int current_fd = open(current_entity, O_WRONLY);
						if (current_fd < 0) {
							fprintf(stderr, "Error in client_handler: current_entity doesn't exist\n");
						}
						int num_write = write(current_fd, send, PACKET_SIZE);
						if (num_write != PACKET_SIZE) {
							fprintf(stderr, "Error in client handler: couldn't write 2048 bytes\n");
						}
						close(current_fd);
					}
				}
				entity = readdir(dir);

			}

			closedir(dir);

		} else if (*type == SAYCONT) {

			uint8_t send[PACKET_SIZE];

	 
			// prepare the sending message
			uint16_t four = RECVCONT;
			// memcpy the value 4 into send address for 2 bytes
			memcpy(send, &four, TYPE_SIZE);
			// memcpy the name into (send + 2) for 256 bytes
			memcpy(send + TYPE_SIZE, identifier, MAX_NAME_LENGTH);
			// memcpy (message + 2) into (send + 2 + 256) for 1789 bytes
			memcpy(send + TYPE_SIZE + MAX_NAME_LENGTH, message + TYPE_SIZE, SAY_MAX - 1);
			// memcpy (message + 2 + 1789 + 256) into (send + 2 + 256 + 1789) for 1 bytes: termination byte
			memcpy(send + TYPE_SIZE + MAX_NAME_LENGTH + SAY_MAX - 1, message + TYPE_SIZE + MAX_NAME_LENGTH + SAY_MAX - 1, 1);
	 
			/* We now scan through the current directory. We write to all the _RD pipes that are not the client's read pipe */
			char identifier_rd[MAX_NAME_LENGTH];
			strcpy(identifier_rd, identifier);
			strcat(identifier_rd, "_RD");

			DIR* dir = opendir(domain);
			if (dir == NULL) {
				fprintf(stderr, "Error in client_handler: couldn't open domain\n");
				return 0;
			}
			struct dirent* entity;
			entity = readdir(dir);
			while (entity != NULL) {

				// If "_RD" is the last three characters of entity->d_name
				if ( is_rd_fifo(entity->d_name) ) {
					// if the current "_RD" file isn't "identifier_RD"
					if (strcmp(entity->d_name, identifier_rd) != 0) {

						// open, write and close to ALL the other clients "_RD" pipes in the same domain
						char current_entity[MAX_NAME_LENGTH];
						strcpy(current_entity, domain);
						strcat(current_entity, "/");
						strcat(current_entity, entity->d_name);

						int current_fd = open(current_entity, O_WRONLY);
		  
						int num_write = write(current_fd, send, PACKET_SIZE);

						if (num_write != PACKET_SIZE) {
							fprintf(stderr, "Error in client handler: couldn't write 2048 bytes\n");
						} 
						close(current_fd);
		  
					}
				}
				entity = readdir(dir);
			} 

			closedir(dir);
		} else if (*type == PONG) {

			// if the actual message was a pong...
			if ( strcmp(message + TYPE_SIZE, "pong") ) {
				// send a signal to the child process
				kill(id2, SIGUSR2);
			}
		}
		close(read_from_write_pipe_fd);
	} // end while 
}
