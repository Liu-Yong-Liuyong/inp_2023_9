/*#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iomanip>*/
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iomanip>
#include <arpa/inet.h>
#include <unistd.h>
#include <zlib.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
using namespace std;


std::string urlEncode(const std::string& input) {
    std::string encoded;
    for (char c : input) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.') {
            encoded += c;
        } else if (c == ' ') {
            encoded += '+';
        } else {
            std::stringstream ss;
            ss << '%' << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(static_cast<unsigned char>(c));
            encoded += ss.str();
        }
    }
    return encoded;
}

int create_socket(const char* server_ip,int port) {
    //int sockfd;
    struct sockaddr_in server_addr;

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        //return "";
    }

    // Set the server's address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  // Change to the appropriate server port
    server_addr.sin_addr.s_addr = inet_addr(server_ip);  // Change to the server's IP address

    // Attempt to connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr) )== -1) {
        perror("Connection failed");
        close(sockfd);
        //return "";
    }

    printf("Connected to the server\n");
    return sockfd;
    //close(sockfd);

}
string sendrequest(const std::string& request,int sockfd){
    if (send(sockfd, request.c_str(),request.length(), 0) == -1) {
        perror("Failed to send the request");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Receive and process the server's response
    char response[4096]; // Adjust the buffer size as needed
    ssize_t bytesReceived;
    string receive="";
    while ((bytesReceived = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytesReceived] = '\0'; // Null-terminate the received data
        //printf("%s", response); // Print the response or process it as needed
        //cout<<response<<"+++++"<<endl;
        receive+=response;     
    }
    return receive;
}




int main() {
    // Step 1: Obtain an OTP (replace <your-student-id> with your student ID)
    //int sockfd=create_socket("172.21.0.4",10001);
    int sockfd=create_socket("140.113.213.213",10314);

    // Construct the HTTP GET request
    //std::string mes = "GET /otp?name=110550039 HTTP/1.1\r\nHost: 172.21.0.4\r\n\r\n";
    std::string mes = "GET /otp?name=110550039 HTTP/1.1\r\nHost: 140.113.213.213\r\n\r\n";

    string receive=sendrequest(mes, sockfd);
    stringstream ss(receive);
    string otp = "";
    string line;
    while (ss >> line) {
        if (line.find("110550039") != string::npos) {
            // cout << line << '\n';
            otp = line;
        }
    }
    cout<<receive<<endl;
    
    cout<<otp<<endl;
    

    string encoded_otp=urlEncode(otp);
    cout<<encoded_otp<<endl;

    
    //string verify="GET /verify?otp=" + encoded_otp+ " HTTP/1.1\r\nHost: 172.21.0.4\r\n\r\n";
    string verify="GET /verify?otp=" + encoded_otp+ " HTTP/1.1\r\nHost: 140.113.213.213\r\n\r\n";
   
    //sockfd = create_socket("172.21.0.4", 10001);
    sockfd=create_socket("140.113.213.213",10314);
    string returned=sendrequest(verify, sockfd);

    cout<<returned<<endl;

    string footer = "\r\n"
                    "--myboundary--\r\n";             
    cout<<"length: "<<otp.length()<<endl;
    cout<<"length2: "<<footer.length()<<endl;

    string temp="--myboundary\r\n"
                "Content-Type: application/octet-stream\r\n"
                "Content-Disposition: form-data; name=\"file\"; filename=\"myfile.ext\"\r\n"
                "Content-Transfer-Encoding: 8bit\r\n" 
                "\r\n";
    cout<<"temp: "<<temp.length()<<endl;

    int num = temp.length() + footer.length() + otp.length();
    string postRequest = "POST /upload HTTP/1.1\r\n"
                    "Host: 140.113.213.213\r\n"
                    "Content-Length: " + to_string(num) + "\r\n"
                    "Content-Type: multipart/form-data; boundary=myboundary\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "--myboundary\r\n"
                    "Content-Type: application/octet-stream\r\n"
                    "Content-Disposition: form-data; name=\"file\"; filename=\"myfile.ext\"\r\n"
                    "Content-Transfer-Encoding: 8bit\r\n" 
                    "\r\n" + otp  + footer;

    string copy=postRequest;

    for(int i=0;i<copy.length();i++){
        if(copy[i]=='\r'){
            copy[i]=' ';
        }
    }
    cout<<copy<<endl;

    //sockfd = create_socket("172.21.0.4", 10001);
    sockfd=create_socket("140.113.213.213",10314);
    
    std::string interres = sendrequest(postRequest, sockfd);
    cout<<interres<<endl;
    
    close(sockfd);
    

    return 0;
}


