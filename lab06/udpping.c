#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/time.h>

#define err_quit(m) { perror(m); exit(-1); }


#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3]
size_t fragment_size = 2048;

int sock;
struct sockaddr_in server_addr;
struct sockaddr_in csin;
socklen_t csinlen = sizeof(csin);

typedef struct {
    unsigned int seq;
    char filename[6];
    size_t num_frag; 
    size_t file_size;
    // Add other necessary fields for your application
} fragment_t;



void send_file(int sock, const char *file_path, const char* filename) {
    unsigned char buf[1024*1024];

    fragment_t *fragment = (fragment_t *)buf;

    FILE *file = fopen(file_path, "rb");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    //get the total file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the number of fragments 確保不整除的時候剩下的是一個封包
    size_t num_fragments = (file_size + fragment_size - 1) / fragment_size;

    for (size_t i = 0; i < num_fragments; ++i) {
        fragment->seq = i; // Set sequence number for the fragment
        //fragment->filename = filename;//確認fragment來自哪一個file

        size_t data_length = strlen(filename) + 1; // Include the null terminator
        //unsigned char buf[data_length];
        strncpy(fragment->filename, filename, data_length);

        fragment->num_frag = num_fragments-1;//

        fragment->file_size = file_size;

        // Read a fragment-sized chunk from the file
        size_t bytes_read = fread(buf + sizeof(*fragment), 1, fragment_size, file);// 

        /*printf("Contents of buffer:\n");
        for (size_t i = 0; i < sizeof(fragment_t) + bytes_read; ++i) {
            printf("%02X ", buf[i]);
        }
        printf("end\n\n");*/
        while(1){
            if (sendto(sock, buf, sizeof(*fragment) + bytes_read, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {//
                perror("sendto");
                break;
            }
            // You may need to adjust the timeout duration based on your network conditions
            struct timeval timeout;
            timeout.tv_sec = 0;  // 5 seconds timeout
            timeout.tv_usec = 5000;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

            char check[10];
            ssize_t bytesReceived1 = recvfrom(sock, check , sizeof(check)-1, 0, (struct sockaddr*) &csin, &csinlen);
            check[bytesReceived1]='\0';

            unsigned int receivedNumber;
            sscanf(check, "%u", &receivedNumber);

            if(receivedNumber==fragment->seq){
                printf("Fragment %zu ACK received.\n", receivedNumber);
                break;
            }/*else {
                printf("Fragment %zu ACK not received. Resending...\n", receivedNumber);
            }*/
        }
        
    }

    fclose(file);
}

void send_files_in_directory(int sock, const char *path) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            count++;
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            char file_path[256]; // Adjust the size as needed
            snprintf(file_path, sizeof(file_path), "%s/%s", path, ent->d_name);

            send_file(sock, file_path,ent->d_name);
            //send_file_name(sock, ent->d_name);

            printf("count: %d",count);
        }
        closedir(dir);
    } else {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    printf("count: %d",count);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "usage: %s <path_to_files> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *path_to_files = argv[1];
    const char *server_port = argv[3];
    const char *server_ip = argv[4];
    

    //int sock;
    //struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        err_quit("socket");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(server_port));

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1)
        err_quit("inet_pton");

    send_files_in_directory(sock, path_to_files);

    close(sock);

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
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define err_quit(m) { perror(m); exit(-1); }

#define NIPQUAD(s)	((unsigned char *) &s)[0], \
					((unsigned char *) &s)[1], \
					((unsigned char *) &s)[2], \
					((unsigned char *) &s)[3]

static int s = -1;
static struct sockaddr_in sin;
static unsigned seq;
static unsigned count = 0;

typedef struct {
	unsigned seq;
	struct timeval tv;
}	ping_t;

double tv2ms(struct timeval *tv) {
	return 1000.0*tv->tv_sec + 0.001*tv->tv_usec;
}

void do_send(int sig) {
	unsigned char buf[1024];
	if(sig == SIGALRM) {
		ping_t *p = (ping_t*) buf;
		p->seq = seq++;
		gettimeofday(&p->tv, NULL);
		if(sendto(s, p, sizeof(*p)+16, 0, (struct sockaddr*) &sin, sizeof(sin)) < 0)
			perror("sendto");
		alarm(1);
	}
	count++;
	if(count > 10) exit(0);
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		return -fprintf(stderr, "usage: %s ... <port> <ip>\n", argv[0]);
	}

	srand(time(0) ^ getpid());

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(strtol(argv[argc-2], NULL, 0));
	if(inet_pton(AF_INET, argv[argc-1], &sin.sin_addr) != 1) {
		return -fprintf(stderr, "** cannot convert IPv4 address for %s\n", argv[1]);
	}

	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");

	signal(SIGALRM, do_send);

	seq = rand() % 0xffffff;
	fprintf(stderr, "PING %u.%u.%u.%u/%u, init seq = %d\n",
		NIPQUAD(sin.sin_addr), ntohs(sin.sin_port), seq);

	do_send(SIGALRM);

	while(1) {
		int rlen;
		struct sockaddr_in csin;
		socklen_t csinlen = sizeof(csin);
		char buf[2048];
		struct timeval tv;
		ping_t *p = (ping_t *) buf;
		
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			continue;
		}

		gettimeofday(&tv, NULL);
		printf("%lu.%06lu %d bytes from %u.%u.%u.%u/%u: seq=%u, time=%.6f ms\n",
				p->tv.tv_sec, p->tv.tv_usec, rlen,
				NIPQUAD(csin.sin_addr), ntohs(csin.sin_port),
				p->seq, tv2ms(&tv)-tv2ms(&p->tv));
	}

	close(s);
}*/



/*typedef struct {
    unsigned seq;
    struct timeval tv;
} ping_t;

void send_file_name(int sock, const char *file_name) {
    size_t data_length = strlen(file_name) + 1; // Include the null terminator
    unsigned char buf[data_length];
    strncpy((char *)buf, file_name, data_length);

    if (send(sock, buf, data_length, 0) < 0)
        perror("send");
}
void send_file(int sock, const char *file_path) {
    unsigned char buf[1024];
    ping_t *p = (ping_t *)buf;
    FILE *file = fopen(file_path, "rb");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    p->seq = rand() % 0xffffff;
    gettimeofday(&p->tv, NULL);

    fread(buf + sizeof(*p), 1, sizeof(buf) - sizeof(*p), file);

    if (send(sock, buf, sizeof(*p) + ftell(file), 0) < 0)
        perror("send");

    fclose(file);
}*/

            // Wait for ACK
            //char ack_message[3];  // Assuming ACK messages are 3 characters long
            //ssize_t recv_bytes;

            // Receive the ACK
           /* recv_bytes = recvfrom(sock, ack_message , sizeof(ack_message), 0, (struct sockaddr*) &csin, &csinlen);


           // Check for ACK or timeout
            if (recv_bytes == sizeof(ack_message) && strncmp(ack_message, "ACK", sizeof(ack_message)) == 0) {
                printf("Fragment %zu ACK received.\n", i);
                break;
            } else {
                printf("Fragment %zu ACK not received. Resending...\n", i);
            }*/