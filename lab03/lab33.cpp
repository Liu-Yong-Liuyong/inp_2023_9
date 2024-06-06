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
const int numRows = 101;
const int numCols = 101;
char mazes[7][19];
char map[101][101];
string path = "";
string path1;
int row_1 =0;
int col_1 =0;
int sockfd;
void down_7(){
    //now down 7;
    for(int i=0;i<1;i++){
        row_1 += 7;
        string com="KKKKKKK";
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                //cout<<"renumstr"<<renumstr[s]<<endl;
                map[i][k]=renumstr[s];
                //cout<<"map:"<<map[i][k]<<endl;
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1+=11;
    }
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;
}
void getwhole_map_1(){
    for(int i=0;i<8;i++){
        cout<<"("<<row_1<<","<<col_1<<")"<<endl;
        string com="LLLLLLLLLLL";//to right
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                map[i][k]=renumstr[s];
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1+=(com.length()-1);
    }
    for(int i=0;i<1;i++){
        cout<<"("<<row_1<<","<<col_1<<")"<<endl;
        string com="LL";//to right
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        //cout<<"33333"<<endl;
        for(int i=0;i<renumstr.size();i++){
            cout<<renumstr[i];
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=90;k<101;k++){
                //cout<<"renumstr"<<renumstr[s]<<endl;
                map[i][k]=renumstr[s];
                //cout<<"map:"<<map[i][k]<<endl;
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=90;k<101;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1+=(com.length()-1);
    }
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;

    //down 7 steps
    for(int i=0;i<1;i++){
        row_1+=7;
        string com="KKKKKKK";
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=90;k<101;k++){
                //cout<<"renumstr"<<renumstr[s]<<endl;
                map[i][k]=renumstr[s];
                //cout<<"map:"<<map[i][k]<<endl;
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=90;k<101;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1-=2;
    }
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;

    for(int i=0;i<1;i++){
        string com="JJ";//to left
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=88;k<99;k++){
                //cout<<"renumstr"<<renumstr[s]<<endl;
                map[i][k]=renumstr[s];
                //cout<<"map:"<<map[i][k]<<endl;
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=88;k<99;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1-=11;
    }
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;

    for(int i=0;i<8;i++){
        string com="JJJJJJJJJJJ";//to left
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            //cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1-11;k<col_1;k++){
                map[i][k]=renumstr[s];
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1-11;k<col_1;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1-=(com.length()-1);
    }
    
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;

}
bool solveMazeDFS(int row, int col, string path,vector<vector<bool>>& visited) {
    // Check if we've reached the exit
    if (map[row][col] == 'E') {
        //cout << "Maze solved within " << steps << " steps." << endl;
        cout << "Path: " << path << endl;
        path1=path;
        return true;
    }


    // Check if the current cell is valid and not a wall or already visited
    if (row < 0 || row >= numRows || col < 0 || col >= numCols || map[row][col] == '#' || visited[row][col]) {
        return false;
    }

    // Mark the current cell as visited
    visited[row][col] = true;

    bool found = false;

    // Explore in all four directions (up, down, left, right)
    if (!found && solveMazeDFS(row - 1, col, path + "W", visited)) found = true;  // Up
    if (!found && solveMazeDFS(row + 1, col, path + "S", visited)) found = true;  // Down
    if (!found && solveMazeDFS(row, col - 1, path + "A", visited)) found = true;  // Left
    if (!found && solveMazeDFS(row, col + 1, path + "D", visited)) found = true;  // Right

    // Unmark the current cell as visited when backtracking
    visited[row][col] = false;

    return found;
}

int main() {
    //int sockfd;
    struct sockaddr_in server_addr;


    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10303);  // 请更改为您要连接的服务器端口
    server_addr.sin_addr.s_addr = inet_addr("140.113.213.213");  // 请替换为您要连接的服务器IP地址

    // 尝试连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    char buffer[5000];
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

                    if (currentLine.find("Enter your move(s)>") != std::string::npos ) {
                        //std::cout << currentLine<<"+++++" << std::endl;
                        doublenewline =true;
                        break;
                    }
                    if(currentLine[0]==' '&&currentLine[4]<='9' &&currentLine[4]>='0'){
                        start=true;
                    }
                        
                    if(start){
                        maze.push_back(currentLine);
                        //cout<<p<<": "<<maze[p]<<endl;
                        p++;
                    }
                    
                    currentLine = "";  // Clear the current line

                } else {
                    // Append characters to the current line
                    currentLine += buffer[i];
                }
            }        
        }
        if(doublenewline){
            //cout<<"+++++"<<endl;
            break;
        }
        
    }
    
    int begin_row=0;
    int d=1;
    for(int i=0;i<7;i++){
        for(int k =0;k<18;k++){
            mazes[i][k]=maze[i][k];
        }
    }
    bool negative = false;
    for(int i=4;i>=2;i--){
        if(mazes[0][i]<=57&&mazes[0][i]>=48){
            begin_row+=(mazes[0][i]-48)*d;
            d*=10;
        }
        if(mazes[0][i]=='-'){/////change
            negative=true;
        }
    }
    cout<<"begin: "<<begin_row<<endl;
    for(int i=0;i<7;i++){
        for(int k =0;k<18;k++){
            cout<<mazes[i][k];
        }
        cout<<endl;
    }
    string command="";
    if(!negative){
        for(int i=0;i<begin_row;i++){
            command += "I";
        }
        command += '\n';
    }else{
        for(int i=0;i<begin_row;i++){
            command += "K";
        }
        command += '\n';
    }
    

    cout<<command<<endl;
    ssize_t bytesSent = send(sockfd, command.c_str(), command.length(), 0);
    if (bytesSent == -1) {
        perror("Error sending data");
        // Handle the error as needed
    } else {
        cout << "Sent " << bytesSent << " bytes to the server." << endl;
    }
    char buffer1[5000];
    ssize_t bytesRead1;
    string recvstr="";
    bool to_right = false;
    while (true) {
        bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
        recvstr += buffer1;
        if (bytesRead1 == 0) {
            cout << "Server closed the connection" << endl;
            break;
        } else if (bytesRead1 == -1) {
            perror("Error receiving data");
            break;
        } else {
            buffer1[bytesRead1] = '\0';
            cout << buffer1 << endl;
            //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
            if(buffer1[8]==' '){
                to_right=true;
            }
            if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                    break;
            }
        }
    }
    cout<<"to_rignt: "<<to_right<<endl;
    //string temp = "";

    
    

    if(!to_right){
        cout<<"to lllllllllllllllll"<<endl;
        //send to left most
        command="J";
        command += '\n';
        bool stop =  false;
        while(!stop){
            //cout<<"*****"<<endl;
            bytesSent = send(sockfd, command.c_str(), command.length(), 0);
            if (bytesSent == -1) {
                perror("Error sending data");
                // Handle the error as needed
            } else {
                //cout << "Sent " << bytesSent << " bytes to the server." << endl;
            }
            recvstr="";
            while (true) {
                bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
                recvstr += buffer1;
                if (bytesRead1 == 0) {
                    cout << "Server closed the connection" << endl;
                    break;
                } else if (bytesRead1 == -1) {
                    perror("Error receiving data");
                    break;
                } else {
                    buffer1[bytesRead1] = '\0';
                    //cout << buffer1 << endl;
                    //cout<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<buffer1[7]<<buffer1[8]<<endl;
                    if(buffer1[8]==' '){
                        stop =true;
                        //break;
                        //cout<<"99999"<<endl;
                    }
                    if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                            break;
                    }
                }
            }
        }

        //return to the start point
        string temp = "";
        vector<char> renumtemp;
        command = "L";
        command += '\n';
        bytesSent = send(sockfd, command.c_str(), command.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                cout << buffer1 << endl;
                temp+=buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                    break;
                }
            }
        }
        for(int i=0;i<temp.length();i++){
            if(temp[i]=='#'||temp[i]=='.'||temp[i]=='*'||(temp[i]=='E'&&temp[i+1]!='n')){
                cout<<temp[i];
                renumtemp.push_back(temp[i]);
            }
        }
        cout<<endl;
        for(int i=0;i<7;i++){
            for(int k=0;k<11;k++){
                map[i][k]=renumtemp[i*11+k];
            }
        }
        for(int i=0;i<7;i++){
            for(int k=0;k<11;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=0;
        col_1=11;

    }
    else{
        string temp0 = "";
        vector<char> renumtemp0;
        cout<<"to rrrrrrrrrrr"<<endl;
        command="L";
        command += '\n';
        bool stop =  false;
        while(!stop){
            //cout<<"*****"<<endl;
            bytesSent = send(sockfd, command.c_str(), command.length(), 0);
            if (bytesSent == -1) {
                perror("Error sending data");
                // Handle the error as needed
            } else {
                //cout << "Sent " << bytesSent << " bytes to the server." << endl;
            }
            recvstr="";
            while (true) {
                bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
                recvstr += buffer1;
                if (bytesRead1 == 0) {
                    cout << "Server closed the connection" << endl;
                    break;
                } else if (bytesRead1 == -1) {
                    perror("Error receiving data");
                    break;
                } else {
                    buffer1[bytesRead1] = '\0';
                    cout << buffer1 << endl;
                    cout<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<buffer1[7]<<buffer1[8]<<endl;
                    temp0 += buffer1;
                    if(buffer1[8]=='#'){/////change
                        stop =true;
                        cout<<"99999"<<endl;
                    }
                    if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                            break;
                    }
                }
            }
        }
        for(int i=0;i<temp0.length();i++){
            if(temp0[i]=='#'||temp0[i]=='.'||temp0[i]=='*'||(temp0[i]=='E'&&temp0[i+1]!='n')){
                cout<<temp0[i];
                renumtemp0.push_back(temp0[i]);
            }
        }
        cout<<endl;
        for(int i=0;i<7;i++){
            for(int k=0;k<11;k++){
                map[i][k]=renumtemp0[i*11+k];
            }
        }
        for(int i=0;i<7;i++){
            for(int k=0;k<11;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=0;
        col_1=11;
    }
    getwhole_map_1();//0-14
    
    down_7();
    getwhole_map_1();//0-28
    
    down_7();
    getwhole_map_1();//0-42
    
    down_7();
    getwhole_map_1();//0-56

    down_7();
    getwhole_map_1();//0-70

    down_7();
    getwhole_map_1();//0-84

    down_7();
    getwhole_map_1();//0-98

    cout<<"("<<row_1<<","<<col_1<<")"<<endl;
    /*for(int i=0;i<98;i++){
        for(int k=0;k<101;k++){
            cout<<map[i][k];
        }
        cout<<endl;
    }*/
    //down 3 steps
    for(int i=0;i<1;i++){
        row_1 += 3;
        string com="KKK";
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                //cout<<"renumstr"<<renumstr[s]<<endl;
                map[i][k]=renumstr[s];
                //cout<<"map:"<<map[i][k]<<endl;
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1+=11;
    }
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;

    //last to rights
    for(int i=0;i<8;i++){
        cout<<"("<<row_1<<","<<col_1<<")"<<endl;
        string com="LLLLLLLLLLL";//to right
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                //cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                map[i][k]=renumstr[s];
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=col_1;k<col_1+11;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1+=(com.length()-1);
    }
    for(int i=0;i<1;i++){
        cout<<"("<<row_1<<","<<col_1<<")"<<endl;
        string com="LL";//to right
        com += '\n';
        string str="";
        vector<char> renumstr;
        ssize_t bytesSent = send(sockfd, com.c_str(), com.length(), 0);
        if (bytesSent == -1) {
            perror("Error sending data");
            // Handle the error as needed
        } else {
            cout << "Sent " << bytesSent << " bytes to the server." << endl;
        }
        char buffer1[5000];
        ssize_t bytesRead1;
        string recvstr="";
        while (true) {
            bytesRead1 = recv(sockfd, buffer1, sizeof(buffer1), 0);
            recvstr += buffer1;
            if (bytesRead1 == 0) {
                cout << "Server closed the connection" << endl;
                break;
            } else if (bytesRead1 == -1) {
                perror("Error receiving data");
                break;
            } else {
                buffer1[bytesRead1] = '\0';
                //cout << buffer1 << endl;
                str += buffer1;
                //cout<<buffer1[0]<<buffer1[1]<<buffer1[2]<<buffer1[3]<<buffer1[4]<<buffer1[5]<<buffer1[6]<<endl;
                if (recvstr.find("Enter your move(s)>") != std::string::npos ) {
                        break;
                }
            }
        }
        for(int i=0;i<str.length();i++){
            if(str[i]=='#'||str[i]=='.'||str[i]=='*'||(str[i]=='E'&&str[i+1]!='n')){
                cout<<str[i];
                renumstr.push_back(str[i]);
            }
        }
        cout<<endl;
        for(int i=0;i<renumstr.size();i++){
            cout<<renumstr[i];
        }
        cout<<endl;
        int s=0;
        for(int i=row_1;i<row_1+7;i++){
            for(int k=90;k<101;k++){
                //cout<<"renumstr"<<renumstr[s]<<endl;
                map[i][k]=renumstr[s];
                //cout<<"map:"<<map[i][k]<<endl;
                s++;
            }
        }
        for(int i=row_1;i<row_1+7;i++){
            for(int k=90;k<101;k++){
                cout<<map[i][k];
            }
            cout<<endl;
        }
        row_1=row_1;
        col_1+=(com.length()-1);
    }
    cout<<"("<<row_1<<","<<col_1<<")"<<endl;

    for(int i=0;i<101;i++){
        for(int k=0;k<101;k++){
            cout<<map[i][k];
        }
        cout<<endl;
    }


    int startRow = -1;
    int startCol = -1;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (map[i][j] == '*') {
                startRow = i;
                startCol = j;
                break;
            }
        }
        if (startRow != -1) {
             break;
        }
    }
     vector<vector<bool>> visited(numRows, vector<bool>(numCols, false));
    // Solve the maze starting from the initial position
    if (solveMazeDFS(startRow, startCol, path,visited)) {
        cout << " Maze_Path: " << path1 << endl;
    } else {
        cout << "Maze cannot be solved." << endl;
    }

    path1 = path1+'\n';
    // Send the path to the server
    ssize_t bytesSent1 = send(sockfd, path1.c_str(), path1.length(), 0);
    if (bytesSent1 == -1) {
        perror("Error sending data");
        // Handle the error as needed
    } else {
        cout << "Sent " << bytesSent1 << " bytes to the server." << endl;
    }

    // Receive and display the server's response
    char buffer2[20000];
    ssize_t bytesRead2;
    
    string total="";
    while (true) {
        bytesRead2 = recv(sockfd, buffer2, sizeof(buffer2), 0);
        if (bytesRead2 == 0) {
            cout << "Server closed the connection" << endl;
            break;
        } else if (bytesRead2 == -1) {
            perror("Error receiving data");
            break;
        } else {
            //cout << buffer2 << endl;
            total += buffer2;
        }
    }
    cout<<total<<endl;

    // Close the socket
    close(sockfd);

    return 0;
}


    
   

