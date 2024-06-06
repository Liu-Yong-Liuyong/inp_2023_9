/*#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	while(1) {
		sleep(100000);
	}
	return 0;
}*/
// dist/server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define BUFFER_SIZE 26214400

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Setup server information
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    //inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));
	//server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }
    
	while (1) {
        // Accept a connection
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }


        // Receive data from the client
        //char buffer[BUFFER_SIZE];
        char *buffer = (char *)malloc(BUFFER_SIZE);//
        ssize_t total_sent=0;
        while(total_sent<BUFFER_SIZE){
            ssize_t sent_bytes = send(client_socket, buffer + total_sent ,BUFFER_SIZE - total_sent, 0);//received_bytes
            if (sent_bytes == -1) {
                perror("Error sending data");
                exit(EXIT_FAILURE);
            }
            else{
                total_sent+=sent_bytes;
            }
        }
        free(buffer);
        // Process the data (for simplicity, just send the same data back)
        /*ssize_t sent_bytes = send(client_socket, buffer,BUFFER_SIZE, 0);//received_bytes
        if (sent_bytes == -1) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }
        free(buffer);*/
        // Close the client socket
        close(client_socket);
    }

    close(server_socket);

    return 0;
}

