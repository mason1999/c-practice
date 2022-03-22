#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../server.h"
int main() {
	char naruto[PACKET_SIZE];
	char sasuke[PACKET_SIZE];
	char kaneki[PACKET_SIZE];
	char haise[PACKET_SIZE];

	uint16_t connect = CONNECT;
	// create the clients: types
	memcpy(naruto, &connect, TYPE_SIZE);
	memcpy(sasuke, &connect, TYPE_SIZE);
	memcpy(kaneki, &connect, TYPE_SIZE);
	memcpy(haise, &connect, TYPE_SIZE);
	// create the client identifiers
	memcpy(naruto + TYPE_SIZE, "naruto", 7);
	memcpy(sasuke + TYPE_SIZE, "sasuke", 7);
	memcpy(kaneki + TYPE_SIZE, "kaneki", 7);
	memcpy(haise + TYPE_SIZE, "haise", 6);
	// create the domains
	memcpy(naruto + TYPE_SIZE + MAX_NAME_LENGTH, "ninja", 6);
	memcpy(sasuke + TYPE_SIZE + MAX_NAME_LENGTH, "ninja", 6);
	memcpy(kaneki + TYPE_SIZE + MAX_NAME_LENGTH, "ghoul", 6);
	memcpy(haise + TYPE_SIZE + MAX_NAME_LENGTH, "ghoul", 6);
	return 0;
}
