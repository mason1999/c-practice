#include "server.h"
#include "functions.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>	   
#include <sys/stat.h>
#include <fcntl.h>	 
#include <unistd.h>	  
#include <dirent.h>        
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>	   

int received_pong;

void sigusr1_handler(int signum) {
	int old_errno = errno;
	// the signal handler waits until a child has terminated
	while ( waitpid(-1, NULL, WNOHANG) != -1) {
		// do nothing until any child has terminated
	}
	errno = old_errno;
}

void sigusr2_handler(int signum) {
	received_pong = 1;
}

void sigchld_handler(int signum) {
	int old_errno = errno;
	// the signal handler waits until a child has terminated
	while ( waitpid(-1, NULL, WNOHANG) != -1) {
		// do nothing until any child has terminated
	}
	errno = old_errno;
}



int main(int argc, char** argv) {

	// create the fifo gevent and check for errors that arent' the existence of gevent. Remember to reset errno
	if (mkfifo("gevent", 0777) == -1 ) {
		if (errno != EEXIST) {
			errno = 0;
			perror("error besides file existing");
			return 1;
		}
   }

	/* message = message we read in from the client
	* identifier = name of client
	* domain = directory they belong to
	* identifier_rd = <domain>/identifier_RD
	* identifier_wr = <domain>/identifier_WR
	* */

	char message[PACKET_SIZE];
	char identifier[MAX_NAME_LENGTH];
	char domain[MAX_NAME_LENGTH];

	char identifier_rd[MAX_NAME_LENGTH];
	char identifier_wr[MAX_NAME_LENGTH];

	int read_from_write_pipe_fd = -1;
	int write_to_read_pipe_fd = -1;

	pid_t id = -1;

	while(1) {

		// clear all the garbage from identifier and domain
		memset(identifier, 0, MAX_NAME_LENGTH);
		memset(domain, 0,  MAX_NAME_LENGTH);

      
		// open gevent in read and write mode
		int gevent_fd_read = open("gevent", O_RDONLY);  

		// read in 2048 into message
		int num_read = read(gevent_fd_read, message, PACKET_SIZE);

		if (num_read < PACKET_SIZE) {
			fprintf(stderr, "couldn't read %d\n", PACKET_SIZE); 
			continue;
		}

		// If type is not CONNECT, then close the file descriptor and try again. 
		uint16_t* type = (uint16_t*)message;
		if (*type != CONNECT) {
			fprintf(stderr, "Error type information wrong in packet read in. Continuing...\n");
			close(gevent_fd_read);
			continue;
		}

		// strncpy MAX_NAME_LENGTH bytes from (message + TYPE_SIZE) into identifier then strncpy MAX_NAME_LENGTH bytes from (message + TYPE_SIZE + MAX_NAME_LENGTH)
		char* temp = message + TYPE_SIZE;
		strncpy(identifier, temp, MAX_NAME_LENGTH);

		temp = message + (TYPE_SIZE + MAX_NAME_LENGTH);
		strncpy(domain, temp, MAX_NAME_LENGTH); 

		// error check the identifier and domain
		if (strlen(domain) > (MAX_NAME_LENGTH - 1)) {
			fprintf(stderr, "Error: domain name is too long. No terminating byte\n");
			return 1;
		}
		if (strlen(identifier) > (MAX_NAME_LENGTH - 4) ) {
			fprintf(stderr, "Error: identifier name is too long. No terminating byte\n");
			return 1;
		}


		// create the domain 
		DIR* dir = opendir(domain);

		if (dir == NULL) {
			// directory doesn't exist.
			errno = 0;

			int ret = mkdir(domain, 0777);

			if (ret == -1) {

				// directory doesn't exist AND invalid domain name. In this case, subdirectories don't exist
				fprintf(stderr, "Directory doens't exist and subdirectories do not exist either\n");
				return 1;
			}
		}

		closedir(dir);

		// create the read and write pipes for the client
		// (i) Make the paths to the fifo
		make_read_path(identifier_rd, domain, identifier);
		make_write_path(identifier_wr, domain, identifier);
		// (ii) Make the write fifos for the client (the fifo WE read from) in the domain and remember to open the client's write pipe for reading
		if (mkfifo(identifier_wr, 0777) == -1) {
			fprintf(stderr, "We coudln't make client_wr fifo\n");
			return 1;
		}
		if (mkfifo(identifier_rd, 0777) == -1) {
			fprintf(stderr, "We couldn't make client_rd fifo\n");
			return 1;
		}

		// open the client_wr pipe
		read_from_write_pipe_fd = open(identifier_wr, O_RDWR); 

		if (read_from_write_pipe_fd < 0) {
			fprintf(stderr, "Note: pipe doesn't exist\n");
			return 1;
		}
		close(read_from_write_pipe_fd); 
      
		// open the client_rd pipe
		write_to_read_pipe_fd = open(identifier_rd, O_RDWR); 

		if (write_to_read_pipe_fd < 0) {
			fprintf(stderr, "Note: pipe doesn't exist\n");
			return 1;
		}

		close(write_to_read_pipe_fd); 

		// implement a sigaction here
		struct sigaction sa;
		sa.sa_handler = sigusr1_handler;

		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

		if (sigaction(SIGUSR1, &sa, NULL) == -1) {
			perror("error in sigaction function");
			return 1;
		}


		// At this point we have the file descriptor from the file we want to read from. we fork and we call our client_handler function
		id = fork();

		if (id == 0) {
			// child process
			break;
		}
		// parent process still here
	}

	/* We now fork one more time. The parent will be the client handler and it's child will handle the job of sending a ping to the client 
	 * every 15 seconds */
	pid_t id2 = fork();

	if (id2 != 0) {

		// implement a sigaction here
		struct sigaction sa;
		sa.sa_handler = sigchld_handler;

		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
			perror("error in sigaction function");
			return 1;
		}


		// client handler. returns 1 on disconnect and 0 on error
		if (client_handler(domain, identifier, identifier_wr, identifier_rd, id2) == 0) {
			fprintf(stderr, "error\n");
		}

	} else {

		// ping pong program

		// implement a sigaction here
		struct sigaction sa;
		sa.sa_handler = sigusr2_handler;

		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

		if (sigaction(SIGUSR2, &sa, NULL) == -1) {
			perror("error in sigaction function");
			return 1;
		}

		// compose the message for ping
		uint8_t ping[PACKET_SIZE];
		uint16_t t = PING;
		memcpy(ping, &t, TYPE_SIZE); 
		memcpy(ping + TYPE_SIZE, "ping", 5);

		// compose the message for DISCONNECT (see else statement below)
		uint8_t disconnect[PACKET_SIZE];
		uint16_t t2 = DISCONNECT;
		memcpy(disconnect, &t2, TYPE_SIZE);

		// inital sleep for 15 seconds
		int done = 0;
		sleep(15);

		// send the ping 
		int file_descriptor = open(identifier_rd, O_WRONLY);
		write(file_descriptor, ping, PACKET_SIZE);
		close(file_descriptor);

		while (!done) {
			received_pong = 0;

			/* If after the 15 seconds the value of pong was changed to 1, then
			 * a pong was received and the global variable "received_pong" was 
			 * changed to 1 to reflect that*/
			sleep(15);
			if (received_pong == 1) {

				// send the ping 
				int file_descriptor = open(identifier_rd, O_WRONLY);
				write(file_descriptor, ping, PACKET_SIZE);
				close(file_descriptor);
				continue;

			} else {
				/* If we don't recieve a pong withing 15 seconds then the client is assumed
				 * to be dead. We change the value of done = 1 to get out of the while loop, 
				 * We then write a disconnect message to the client_wr pipe and we send a signal
				 * to the client_handler (using get_ppid) to wait for the executing to stop
				 * so no zombie processes are made */
				done = 1;

				int writing_file_descriptor = open(identifier_wr, O_WRONLY);
				write(writing_file_descriptor, disconnect, PACKET_SIZE);
				close(writing_file_descriptor);
				kill(getppid(), SIGCHLD);
			}
		}

	}


	return 0;
}
