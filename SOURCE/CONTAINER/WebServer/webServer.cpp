/*
 * This code is adapted from the IncludeOS Acorn web server example.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <fstream>


#define SERVER_PORT 80
#define BUFFER_SIZE 2048

bool isIndexRequested(char receiveBuffer[]) {
	// if only / is requested
	if(receiveBuffer[4] == '/' && receiveBuffer[5] == ' ')
		return true;
	
	// if only /index.html is requested
	char request[25];
	memcpy(request, receiveBuffer, 24);
	request[25] = '\0';
	
	char *output = NULL;
	output = strstr(request," /index.html ");
	if(output)
		return true;
	
	return false;
}

int main(int argc, char *argv[])
{
	// define address and socket variables
	struct sockaddr_in serverAddress;
	int serverSocket;
	int clientSocket;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int bytesReceivedLength;
    char receiveBuffer[BUFFER_SIZE];
	
	// Retrieve the HTML page from the disk
	std::ifstream file("index.html", std::ios::binary | std::ios::ate);
	std::streamsize indexLength = file.tellg();
	file.seekg(0, std::ios::beg);
	char indexArray[indexLength];
	file.read(indexArray, indexLength);
	
	// creating error message
	std::string error = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body>Page not found</body></html>\r\n";
	int errorLength = error.length(); 
	// declaring character array
	char errorArray[errorLength+1];
	strcpy(errorArray, error.c_str()); 

	
	// creating TCP socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}
	
	// defining server address properties
	memset((void *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
	//serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(SERVER_PORT);
	
	// binding to UDP socket
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("bind failed");
		return 0;
	}
	
	// set the socket for listening (queue backlog of 5)
	if (listen(serverSocket, 5) < 0) {
		perror("listen failed");
		return 0;
	}
	
	printf("Service IP address is %s on port %d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
	
	for (;;) {
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
		
		bytesReceivedLength = read(clientSocket, receiveBuffer, BUFFER_SIZE);
		
		printf("####### INCOMMING REQUEST #######\n");
		printf("Received a connection from: %s port %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
		printf("received %d bytes\n", bytesReceivedLength);
		
		if (bytesReceivedLength > 0) {

			// TODO respond only to GET and / or /index.html
			if(receiveBuffer[0] == 'G' && isIndexRequested(receiveBuffer) ) {	//request is not a GET
				// debug output
				printf("%s", receiveBuffer);
				
				// send reply
				//printf("%s", indexArray);
				int nbytes = write(clientSocket, indexArray, indexLength);
				printf("%d sent to client\n", nbytes);
			} else {
				printf("%s", receiveBuffer);
				int nbytes = write(clientSocket, errorArray, (errorLength+1));
				printf("%d bytes sent to client\n", nbytes);
			}
			
			printf("#################################\n");
			shutdown(clientSocket, 2);
		}
	}
}
