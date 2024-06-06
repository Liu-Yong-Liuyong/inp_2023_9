#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define err_quit(m) { perror(m); exit(-1); }
struct sockaddr_in csin;
int s;
struct sockaddr_in sin;
int current_seq = 0;

typedef struct {
    unsigned int seq;
    char filename[6];
    size_t num_frag;
    size_t file_size;
    // Add other necessary fields for your application
} fragment_t;

void process_fragment(const char *buf, size_t len, const char *store_path) {
    if (len < sizeof(fragment_t)) {
        printf("Invalid fragment size\n");
        return;
    }

    fragment_t *received_fragment = (fragment_t *)buf;
    //unsigned int seq = ntohl(received_fragment->seq); // Convert sequence number to host byte order
    const char* name = received_fragment->filename;

    //printf("%s\n",name);

    // Process the fragment based on its sequence number
    // Add your logic to handle the received fragment here
    

    // Create a file path based on the store_path and filename
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", store_path, name);

    FILE *file = fopen(filepath, "ab"); // Open file in binary append mode

    if (file == NULL) {
        perror("fopen");
        return;
    }else{
        printf("Received fragment with sequence number: %u, filename: %s \n", received_fragment->seq, name);
        // Write the fragment data to the file
        if(received_fragment->seq == current_seq){
            fwrite(buf + sizeof(fragment_t), 1, len-sizeof(fragment_t) , file);//- sizeof(fragment_t)
            printf("data written size: %ld\n",len-sizeof(fragment_t));
            current_seq ++;
        }
        //if current fragment is the last fragment
        if(received_fragment->seq == received_fragment->num_frag){
            current_seq = 0;//initialize
            // Print the current file size
            fseek(file, 0, SEEK_END); // Move to the end of the file
            size_t file_size = ftell(file); // Get the file size
            fseek(file, 0, SEEK_SET); // Restore the file position
            printf("Current file size: %ld bytes\n", file_size);
            printf("Origin size: %ld \n", received_fragment->file_size);
        }
        
        fclose(file);

        char check[10];
        snprintf(check, sizeof(check), "%u", received_fragment->seq);
        if (sendto(s, check, sizeof(check), 0, (struct sockaddr *)&csin, sizeof(csin)) < 0) {
            perror("sendto");
            // Handle error sending ACK if needed
        }

    }

}


int main(int argc, char *argv[]) {
    //int s;
    //struct sockaddr_in sin;

    if (argc < 4) {
        return -fprintf(stderr, "usage: %s <port>\n", argv[0]);
    }

    const char* store_path = argv[1];

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(strtol(argv[argc - 1], NULL, 0));
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        err_quit("socket");

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        err_quit("bind");

    while (1) {
        //struct sockaddr_in csin;
        socklen_t csinlen = sizeof(csin);
        unsigned char buf[1024*1024];
        size_t rlen;

        //printf("Waiting for data...\n");

        rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&csin, &csinlen);

        /*printf("\nContents of buffer on server:\n");
        for (size_t i = 0; i < rlen; ++i) {
            printf("%02X ", buf[i]);
        }
        printf("end\n\n");*/

        if (rlen < 0) {
            perror("recvfrom");
            continue;
        }else{
            //process_fragment(buf, rlen);
            process_fragment(buf, rlen, store_path);
            buf[rlen] = '\0'; // Null-terminate the received data
        }

    }

    close(s);

    return 0;
}

/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define err_quit(m) { perror(m); exit(-1); }

int main(int argc, char *argv[]) {
	int s;
	struct sockaddr_in sin;

	if(argc < 2) {
		return -fprintf(stderr, "usage: %s ... <port>\n", argv[0]);
	}

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(strtol(argv[argc-1], NULL, 0));

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");

	if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0)
		err_quit("bind");

	while(1) {
		struct sockaddr_in csin;
		socklen_t csinlen = sizeof(csin);
		char buf[2048];
		int rlen;
		
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			break;
		}

		sendto(s, buf, rlen, 0, (struct sockaddr*) &csin, sizeof(csin));
	}

	close(s);
}*/



/*void hexStringToFormattedString(const char *hexString, char *formattedString) {
    while (*hexString != '\0') {
        if (*hexString != '\\') {
            sprintf(formattedString, "%s%x", formattedString, (unsigned char)(*hexString));
        }
        hexString++;
    }
}

void process_fragment(const char *buf, size_t len) {
    if (len < sizeof(fragment_t)) {
        printf("Invalid fragment size\n");
        return;
    }

    fragment_t *received_fragment = (fragment_t *)buf;
    unsigned int seq = ntohl(received_fragment->seq); // Convert sequence number to host byte order

    // Process the fragment based on its sequence number
    // Add your logic to handle the received fragment here
    printf("Received fragment with sequence number: %u\n", seq);
}


char formattedString[4096] = ""; // Adjust the size as needed
        hexStringToFormattedString(buf, formattedString);

        printf("Received data from %s:%d:\n", inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));
        printf("%s\n", formattedString);*/



        /*char ack_message[] = "ACK"; // You can customize your ACK message
    if (sendto(s, ack_message, sizeof(ack_message), 0, (struct sockaddr *)&csin, sizeof(csin)) < 0) {
        perror("sendto");
        // Handle error sending ACK if needed
    }*/
