/*#include <stdio.h>

int main(int argc, char *argv[]) {
	printf("# RESULTS: delay = 10.12 ms, bandwidth = 100.11 Mbps\n");
	return 0;
}*/
// dist/client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define LARGE_BUFFER_SIZE 26214400//2586543

int main() {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    // Setup server information
    struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);  // Change to the appropriate server port
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Change to the server's IP address


	// Add these lines before and after the connect call
    struct timeval start, end;
    gettimeofday(&start, NULL);
	//printf("Connecting to the server...\n");
	if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
		perror("Error connecting to server");
		exit(EXIT_FAILURE);
	}
	//printf("Connected to the server\n");

    // Dynamically allocate a large buffer
    char *large_buffer = (char *)malloc(LARGE_BUFFER_SIZE);
    if (large_buffer == NULL) {
        perror("Error allocating large buffer");
        exit(EXIT_FAILURE);
    }

    
    ssize_t total_received = 0;
    float delay3;
    // float delay;
    bool flag = false;
    float save[1024];
    int i=0;
    //float addtime;
    while (total_received < LARGE_BUFFER_SIZE) {
        if(!flag){
            gettimeofday(&end, NULL);
            flag = true;
        }
        
        struct timeval s1, e1;
        gettimeofday(&s1, NULL);
        ssize_t received_bytes = recv(client_socket, large_buffer + total_received, LARGE_BUFFER_SIZE - total_received, 0);
        if (received_bytes == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        } else if (received_bytes == 0) {
            // Connection closed by the other end
            break;
        } else {
            total_received += received_bytes;
        }
        gettimeofday(&e1, NULL);
        delay3 = (((e1.tv_sec - s1.tv_sec) * 1000000LL + (e1.tv_usec - s1.tv_usec))) / 1000000.0;
        save[i] = received_bytes*8/delay3/1000000;
        i++;  
    }
    free(large_buffer);
    close(client_socket);
    //gettimeofday(&end, NULL);
    float total=0;
    int num=0;
    for(int k=0;k<i;k++){
        //printf("save[%d] = %f ",k,save[k]);
        if(save[k]>580 || save[k]<20){
            continue;
        }
        else{
            total += save[k];
            num++;
        }
        
    }
    //printf("\ntotal average: %f\n",total/num);
    float delayt = (((end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_usec - start.tv_usec))/2)/1000; //milisecond
    //printf("delay3: %f\n",delay3);
    

    printf("# RESULTS: delay = %f ms, bandwidth = %f Mbps\n", delayt, total/num);
    //close(client_socket);

    return 0;
}

