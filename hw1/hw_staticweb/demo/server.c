
//mine
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>




#define MAX_REQUEST_SIZE 8192

#define errquit(m)  { perror(m); exit(-1); }

static int port_http = 80;
static int port_https = 443;
static const char *docroot = "/html";

void handle_request(int client_socket);

void url_decode(char *str) {
    int i, j = 0;
    char decoded[256]; // Assuming a maximum length for the decoded string

    for (i = 0; str[i]; i++) {
        if (str[i] == '%' && isxdigit(str[i + 1]) && isxdigit(str[i + 2])) {
            // If '%' followed by two valid hex digits, decode the character
            int hex_value;
            sscanf(&str[i + 1], "%2x", &hex_value);
            decoded[j++] = hex_value;
            i += 2;
        } else {
            // Copy unchanged
            decoded[j++] = str[i];
        }
    }

    // Null-terminate the decoded string
    decoded[j] = '\0';

    // Copy the decoded string back to the original buffer
    strcpy(str, decoded);
}

int main(int argc, char *argv[]) {
    int s;
    struct sockaddr_in sin;

    if (argc > 1) { port_http = strtol(argv[1], NULL, 0); }
    if (argc > 2) { if ((docroot = strdup(argv[2])) == NULL) errquit("strdup"); }
    if (argc > 3) { port_https = strtol(argv[3], NULL, 0); }

    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) errquit("socket");

    do {
        int v = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
    } while (0);

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_http);
    if (bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0) errquit("bind");
    if (listen(s, SOMAXCONN) < 0) errquit("listen");

    printf("Server is listening on port %d...\n", port_http);

    while (1) {
        int c;
        struct sockaddr_in csin;
        socklen_t csinlen = sizeof(csin);

        if ((c = accept(s, (struct sockaddr*) &csin, &csinlen)) < 0) {
            perror("accept");
            continue;
        }

        handle_request(c);

        close(c);
    }

    return 0;
}


void handle_request(int client_socket) {
    char request[MAX_REQUEST_SIZE];
    ssize_t bytes_received = read(client_socket, request, sizeof(request) - 1);//recv(client_socket, request, sizeof(request) - 1, 0)

    if (bytes_received <= 0) {
        // Connection closed or error
        close(client_socket);
        return;
    }

    // Null-terminate the received data to treat it as a string
    request[bytes_received] = '\0';

    // Parse the HTTP request to get the requested file path
    char method[10], path[255];
    char temp[255];
    sscanf(request, "%s %s %s", method, path, temp);

    if(path[1]=='%'){
        url_decode(path);
    }

    // Check if there are query parameters in the path
    char *query_params = strchr(path, '?');
    if (query_params != NULL) {
        // Separate the path and query parameters
        *query_params = '\0';  // Null-terminate the path
        query_params++;        // Move to the start of the query parameters
    }


    // We only handle GET requests
    if (strcmp(method, "GET") != 0) {
        // Respond with 501 Not Implemented for unsupported methods
        //const char *response = "HTTP/1.0 501 Not Implemented\r\n\r\n";
        const char *response = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>\r\n";
        send(client_socket, response, strlen(response), 0);
        return;
    }
    


    // Concatenate the requested path with the document root
    char file_path[265];
    snprintf(file_path, sizeof(file_path), "%s%s", docroot, path);

    // Check if the path is a directory without a trailing slash
    struct stat path_stat;
    //ssize_t content_length = path_stat.st_size;//
    if (stat(file_path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode) && path[strlen(path) - 1] != '/') {
        // Redirect to the version with a trailing slash
        const char *redirect_response = "HTTP/1.0 301 Moved Permanently\r\nLocation: %s/\r\n\r\n";
        dprintf(client_socket, redirect_response, path);
        return;
    }

    // Open the requested file
    //int file_fd = open(file_path, O_RDONLY);
    //if (file_fd < 0) {
        // Respond with 404 Not Found if the file is not present
      //  const char *not_found_response = "HTTP/1.0 404 Not Found\r\n\r\n";
        //send(client_socket, not_found_response, strlen(not_found_response), 0);
        //return;
    //}

    FILE *file_ptr = fopen(file_path, "rb");  // Open in binary mode
    //if (file_ptr == NULL) {
        // Respond with 404 Not Found if the file is not present
        //const char *not_found_response = "HTTP/1.0 404 Not Found\r\n\r\n";
        //send(client_socket, not_found_response, strlen(not_found_response), 0);
        //return;
    //}

    
    // Determine the MIME type based on the file extension
    const char *ext = strrchr(file_path, '.');
    const char *mime_type = "application/octet-stream";  // Default MIME type
    if (ext != NULL) {
        if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
            mime_type = "text/html";
        } else if (strcmp(ext, ".txt") == 0) {
            mime_type = "text/plain;charset=utf-8";
        } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
            mime_type = "image/jpeg";
        } else if (strcmp(ext, ".png") == 0) {
            mime_type = "image/png";
        } else if (strcmp(ext, ".mp3") == 0) {
            mime_type = "audio/mpeg";
        }
        // Add more extensions and MIME types as needed
    }


    //dprintf(client_socket, "HTTP/1.0 200 OK\r\n");
    //dprintf(client_socket, "Content-Type: %s\r\n\r\n", mime_type);
    // Check for the presence of index.html in directories
    struct stat index_stat;
    char index_path[PATH_MAX];
	//snprintf(index_path, sizeof(index_path), "%s/index.html", file_path);
    if (path[strlen(path) - 1] == '/') {//strcmp(path, "/") == 0
        snprintf(index_path, sizeof(index_path), "%sindex.html", file_path);

        if (stat(index_path, &index_stat) == 0 && S_ISREG(index_stat.st_mode)) {
            //int index_fd = open(index_path, O_RDONLY);
            FILE *index_fd = fopen(index_path, "rb");
            if (index_fd == NULL) {
                // Respond with 404 Not Found if the index file is not present
                //const char *not_found_response = "HTTP/1.0 404 Not Found\r\n\r\n";
                const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n";
                send(client_socket, not_found_response, strlen(not_found_response), 0);
                return;
            }
            fseek(index_fd, 0, SEEK_END);
            long content_length = ftell(index_fd);
            rewind(index_fd);

            // Read the index file content and send it to the client
            dprintf(client_socket, "HTTP/1.0 200 OK\r\n");
            dprintf(client_socket, "Content-Type: %s\r\n", "text/html");
            dprintf(client_socket, "Content-Length: %ld\r\n\r\n", content_length);
            char index_buffer[8192];
            ssize_t index_bytes_read;
            while ((index_bytes_read = fread(index_buffer,1,sizeof(index_buffer),index_fd)) > 0) {
                //send(client_socket, index_buffer, index_bytes_read, 0);
                write(client_socket,index_buffer,index_bytes_read);
            }

            // Close the index file
            fclose(index_fd);
        } else {
            // Respond with 403 Forbidden if index.html is not present in the directory
            //const char *forbidden_response = "HTTP/1.0 403 Forbidden\r\n\r\n";
            const char *forbidden_response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n<html><body><h1>403 Forbidden</h1></body></html>\n";
            send(client_socket, forbidden_response, strlen(forbidden_response), 0);
            
        }
    }else{
        if (file_ptr == NULL) {
            // Respond with 404 Not Found if the file is not present
            //const char *not_found_response = "HTTP/1.0 404 Not Found\r\n\r\n";
            const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n";
            send(client_socket, not_found_response, strlen(not_found_response), 0);
            return;
        }
        fseek(file_ptr, 0, SEEK_END);
        long content_length = ftell(file_ptr);
        rewind(file_ptr);
        dprintf(client_socket, "HTTP/1.0 200 OK\r\n");
        dprintf(client_socket, "Content-Type: %s\r\n", mime_type);//\r\n\r\n
        dprintf(client_socket, "Content-Length: %ld\r\n\r\n", content_length);
        char buffer[8192];
        ssize_t bytes_read;
        //size_t total_bytes_sent = 0;
        while ((bytes_read = fread(buffer,1,sizeof(buffer),file_ptr)) > 0) {//read(file_fd, buffer, sizeof(buffer))
            //send(client_socket, buffer, bytes_read, 0);
            write(client_socket,buffer,bytes_read);
            //total_bytes_sent += bytes_read;
        }
    }
    
    
    // Close the file
    //close(file_fd);
    fclose(file_ptr);

    
   
	
}








