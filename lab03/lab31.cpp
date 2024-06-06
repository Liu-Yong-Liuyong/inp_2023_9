#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>

using namespace std;

// Define the maze dimensions
const int numRows = 7;
const int numCols = 11;
char mazes[7][11];
string path = "";
string path1;
bool solveMazeDFS(int row, int col, string path, int steps, int maxSteps, vector<vector<bool>>& visited) {
    // Check if we've reached the exit
    if (mazes[row][col] == 'E' && steps <= maxSteps) {
        cout << "Maze solved within " << steps << " steps." << endl;
        cout << "Path: " << path << endl;
        path1=path;
        return true;
    }

    // If we have exceeded the maximum steps, return false
    if (steps >= maxSteps) {
        return false;
    }

    // Check if the current cell is valid and not a wall or already visited
    if (row < 0 || row >= numRows || col < 0 || col >= numCols || mazes[row][col] == '#' || visited[row][col]) {
        return false;
    }

    // Mark the current cell as visited
    visited[row][col] = true;

    bool found = false;

    // Explore in all four directions (up, down, left, right)
    if (!found && solveMazeDFS(row - 1, col, path + "W", steps + 1, maxSteps, visited)) found = true;  // Up
    if (!found && solveMazeDFS(row + 1, col, path + "S", steps + 1, maxSteps, visited)) found = true;  // Down
    if (!found && solveMazeDFS(row, col - 1, path + "A", steps + 1, maxSteps, visited)) found = true;  // Left
    if (!found && solveMazeDFS(row, col + 1, path + "D", steps + 1, maxSteps, visited)) found = true;  // Right

    // Unmark the current cell as visited when backtracking
    visited[row][col] = false;

    return found;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;


    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10301);  // 请更改为您要连接的服务器端口
    server_addr.sin_addr.s_addr = inet_addr("140.113.213.213");  // 请替换为您要连接的服务器IP地址

    // 尝试连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    /*char buffer;
    ssize_t bytesRead;

    while (bytesRead = recv(sockfd, &buffer, 1, 0) > 0) {
        if (buffer == '\n') {
            printf("\n");  // Print a newline for each message
        } else {
            printf("%c", buffer);  // Print individual characters
        }
    }*/

    // Receive and display messages from the server in real-time

    char buffer[1024];
    ssize_t bytesRead;
    string currentLine; 
    vector<string> maze;
    bool start = false;
    //char mazes[7][11];

    int p=0;
    bool doublenewline =false;
    while (true) {
        bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytesRead == 0) {
            std::cout << "Server closed the connection" << std::endl;
            break;
        } else if (bytesRead == -1) {
            perror("Error receiving data");
            break;
        } else {
            for (int i = 0; i < bytesRead; i++) {
                //cout<<"++++++"<<endl;
                if (buffer[i] == '\n') {
                    // Display the current line
                    std::cout << currentLine << std::endl;

                    if (currentLine.find("[16 steps left] Enter your move(s)>") != std::string::npos ) {
                        //std::cout << currentLine<<"+++++" << std::endl;
                        doublenewline =true;
                        break;
                    }
                    if(currentLine.find("###########")!= std::string::npos){
                        start = true;
                    }
                        
                    if(start){
                        maze.push_back(currentLine);
                        //cout<<maze[p]<<endl;
                        p++;
                    }
                    
                    currentLine = "";  // Clear the current line

                } else {
                    // Append characters to the current line
                    currentLine += buffer[i];
                    /*if(buffer[i]=='#'){
                        start = true;
                    }
                    if(buffer[i]=='['){
                        start = false;
                    }  */
                }
            }        
        }
        if(doublenewline){
            //cout<<"+++++"<<endl;
            break;
        }
        
    }
    

    for(int i=0;i<7;i++){
        for(int k =0;k<11;k++){
            mazes[i][k]=maze[i][k];
        }
    }
    for(int i=0;i<7;i++){
        for(int k =0;k<11;k++){
            cout<<mazes[i][k];
        }
        cout<<endl;
    }

    int startRow = -1;
    int startCol = -1;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (mazes[i][j] == '*') {
                startRow = i;
                startCol = j;
                break;
            }
        }
        if (startRow != -1) {
            break;
        }
    }

    //string path = "";  // Initialize the path as an empty string
    int maxSteps = 16;
    //string path = "";
    vector<vector<bool>> visited(numRows, vector<bool>(numCols, false));
    // Solve the maze starting from the initial position
    if (solveMazeDFS(startRow, startCol, path,0,maxSteps,visited)) {
        cout << "Maze solved. Path: " << path1 << endl;
    } else {
        cout << "Maze cannot be solved." << endl;
    }
    
    path1 = path1+'\n';
    // Send the path to the server
    ssize_t bytesSent = send(sockfd, path1.c_str(), path1.length(), 0);
    if (bytesSent == -1) {
        perror("Error sending data");
        // Handle the error as needed
    } else {
        cout << "Sent " << bytesSent << " bytes to the server." << endl;
    }

    // Receive and display the server's response
    char buffer1[1024];
    ssize_t bytesRead1;
    /*cout<<"33333"<<endl;
    
    cout<<"33333"<<endl;
    bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
    cout<<bytesRead1<<endl;
    buffer1[bytesRead1] = '\0';
    cout<<buffer1<<endl;*/
    while (true) {
        bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
        if (bytesRead1 == 0) {
            cout << "Server closed the connection" << endl;
            break;
        } else if (bytesRead1 == -1) {
            perror("Error receiving data");
            break;
        } else {
            cout << "Server response: " << buffer1 << endl;
        }
    }
   



    // Close the socket
    close(sockfd);

    return 0;
}

