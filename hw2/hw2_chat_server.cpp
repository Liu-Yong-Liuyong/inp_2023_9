#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <utility>
#include <set>

std::vector<std::pair<int, bool>> clientStatusVector;

const int BUFFER_SIZE = 1024;
const int MAX_CLIENTS = 1001;
bool exit_true = false;
struct UserInfo {
    std::string username;
    std::string password;
    int chatRoomnum ;
    bool isLoggedIn;
    bool isLogOut;
    std::string status;
    int clientSocket;
};
struct ChatRoom {
    int roomNumber;
    std::string owner;
    std::vector<std::string> chatHistory;
    std::string pinMessage;
    std::set<int> participants;
};
std::vector<std::string> filteredWords = {
    "==",
    "Superpie",
    "hello",
    "Starburst Stream",
    "Domain Expansion"
};

std::vector<ChatRoom> chatRooms;

std::vector<UserInfo> users;

std::string filterMessage(const std::string& message) {
    std::string filteredMessage = message;
    // Iterate through each keyword
    for (const std::string& keyword : filteredWords) {
        // Convert both message and keyword to lowercase for case-insensitive matching
        std::string lowerMessage = message;
        std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
        std::string lowerKeyword = keyword;
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

        // Find the keyword in the message and replace it with '*' characters if found
        size_t pos = lowerMessage.find(lowerKeyword);
        while (pos != std::string::npos) {
            // Replace the keyword with '*' characters of the same length
            filteredMessage.replace(pos, keyword.length(), keyword.length(), '*');

            // Find the next occurrence, if any
            pos = lowerMessage.find(lowerKeyword, pos + keyword.length());
        }
    }

    return filteredMessage;
}

void send_welcome_message(int client_socket) {
    const char welcome_message[] = 
    "*********************************\n"
    "** Welcome to the Chat server. **\n"
    "*********************************\n";
    
    send(client_socket, welcome_message, strlen(welcome_message), 0);
}

std::string handle_command(int client_socket, std::string command) {
    // Split command into tokens
    std::istringstream iss(command);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>());

    if (tokens.empty()) {
        return "Invalid command format.\n";
    }
    bool currentuserenter = false;
    int currentchatroom;
    std::string currentuser;
    for(auto& user : users){
        if(user.isLoggedIn && user.clientSocket == client_socket){
            if(user.chatRoomnum != 0){
                currentuserenter = true;//已經進到某個聊天室
                currentchatroom = user.chatRoomnum;
                currentuser = user.username;
            }
        }
    }
    if((tokens[0] == "%") && !currentuserenter){
        if (tokens[1] == "register") {
            if (tokens.size() != 4) {
                return "Usage: register <username> <password>\n";
            }
            for (const auto &user : users) {
                if (user.username == tokens[2]) {
                    return "Username is already used.\n";
                }
            }
            users.push_back({tokens[2], tokens[3], 0, false, false, "offline", 0/*client_socket*/});
            return "Register successfully.\n";
        } 
        else if (tokens[1] == "login") {
            if (tokens.size() < 4) {
                return "Usage: login <username> <password>\n";
            }
            bool isInUse=false;
            for (auto& pair : clientStatusVector) {
                if (pair.first == client_socket && pair.second == true) {
                    isInUse = true;
                }
            }
            for (auto& user : users) {
                if (user.username == tokens[2]) {
                    // Check if the user is already logged in elsewhere
                    /*if (user.isLoggedIn) {//
                        std::cout<<"88888"<<std::endl;
                        //return "Please logout first.\n";
                        return "Login failed.\n"; //account is already logged by another client
                    }*/
                    /*for (auto& pair : clientStatusVector) {
                        if (pair.first == user.clientSocket && pair.second == true) {
                            isInUse = true;
                        }
                    }*/
                    if(!isInUse && !user.isLoggedIn){
                        // Verify the password
                        if (user.password == tokens[3]) {
                            // Mark the user as logged in for this client connection
                            user.isLoggedIn = true;
                            user.status = "online";
                            user.clientSocket = client_socket;
                            clientStatusVector.push_back(std::make_pair(client_socket, true));
                            return "Welcome, " + tokens[2] + ".\n";
                        } else {
                            return "Login failed.\n"; // Password is incorrect
                        }
                    }else{
                        if(isInUse){
                            return "Please logout first.\n";//if a client has logged in an account
                        }
                        if(user.isLoggedIn){
                            return "Login failed.\n"; //account is already logged by another client
                        }
                        
                    }
                    /*if (user.isLoggedIn) {//
                        return "Login failed.\n"; //account is already logged by another client
                    }*/
                    
                }
            }
        
            // If the username doesn't exist
            return "Login failed.\n"; 

        } 
        else if (tokens[1] == "logout") {
            // Check if the command has any redundant parameters
            if (tokens.size() != 2) {
                return "Usage: logout\n";
            }

            // Find the user and check if they are logged in
            bool userFoundAndLoggedOut = false;
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    user.isLoggedIn = false;  // Mark user as logged out
                    user.isLogOut = true;
                    userFoundAndLoggedOut = true;
                    user.status = "offline";
                    int i=0;
                    for (auto& pair : clientStatusVector) {
                        if (pair.first == user.clientSocket && pair.second == true) {
                            clientStatusVector.erase(clientStatusVector.begin()+i);
                        }
                        i++;
                    }
                    user.clientSocket = 0;//logout之後將clientsocket變為0
                    // Return the logout message with the username
                    return "Bye, " + user.username + ".\n";
                }
            }

            // If the user was not found or was not logged in
            if (!userFoundAndLoggedOut) {
                return "Please login first.\n";
            }
        } 
        else if (tokens[1] == "exit") {//大問題
            if (tokens.size() != 2) {
                return "Usage: exit\n";
            }

            // Find the user and check if they are logged in
            bool userFoundAndLoggedOut = false;
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    user.isLoggedIn = false;  // Mark user as logged out
                    user.isLogOut = true;
                    userFoundAndLoggedOut = true;
                    user.status = "offline";
                    int i=0;
                    for (auto& pair : clientStatusVector) {
                        if (pair.first == user.clientSocket && pair.second == true) {
                            clientStatusVector.erase(clientStatusVector.begin()+i);
                        }
                        i++;
                    }
                    user.clientSocket = 0;//logout之後將clientsocket變為0
                    exit_true = true;
                    // Return the logout message with the username
                    return "Bye, " + user.username + ".\n";
                }
            }

            // If the user was not found or was not logged in
            if (!userFoundAndLoggedOut) {
                exit_true = true;
                return "";
            }
        } 
        else if (tokens[1] == "whoami") {
            // Check if the command has any redundant parameters
            if (tokens.size() != 2) {
                return "Usage: whoami\n";
            }

            // Find the user and check if they are logged in
            for (const auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    return user.username + "\n";
                }
            }

            // If the user was not found or was not logged in
            return "Please login first.\n";
        } 
        else if (tokens[1] == "set-status") {
            // Check for correct number of parameters
            if (tokens.size() != 3) {
                return "Usage: set-status <status>\n";
            }

            // Check if the user is logged in
            bool isLoggedIn = false;
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    isLoggedIn = true;
                    // Validate and set the status
                    if (tokens[2] == "online" || tokens[2] == "offline" || tokens[2] == "busy") {
                        user.status = tokens[2];
                        return user.username + " " + user.status + "\n";
                    } else {
                        return "set-status failed\n";
                    }
                }
            }

            // If the user is not logged in
            if (!isLoggedIn) {
                return "Please login first.\n";
            }
        } 
        else if (tokens[1] == "list-user") {
            // Check for correct number of parameters
            if (tokens.size() != 2) {
                return "Usage: list-user\n";
            }

            // Check if the user is logged in
            bool isLoggedIn = false;//會出問題
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    isLoggedIn = true;
                    break;
                }
            }

            // If the user is not logged in
            if (!isLoggedIn) {
                return "Please login first.\n";
            }

            // Sort users by their usernames
            std::sort(users.begin(), users.end(), [](const UserInfo& a, const UserInfo& b) {
                return a.username < b.username;
            });

            // Construct the list of users and their statuses
            std::string userList;
            for (const auto& user : users) {
                userList += user.username + " " + user.status + "\n";
            }

            return userList;
        }
        else if(tokens[1] == "enter-chat-room"){
            if(tokens.size() != 3){
                return "Usage: enter-chat-room <number>\n";
            }
            int roomnumber= std::stoi(tokens[2]);
            if(roomnumber<1 || roomnumber>100){
                return "Number " + tokens[2] + " is not valid.\n";
            }
            bool isLoggedIn = false;
            std::string username;
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    isLoggedIn = true;
                    user.chatRoomnum = roomnumber;
                    username = user.username;
                    break;
                }
            }
            // If the user is not logged in
            if (!isLoggedIn) {
                return "Please login first.\n";
            }

            bool roomExists = false;
            for(auto& room : chatRooms){
                if(room.roomNumber == roomnumber){
                    roomExists = true;
                    room.participants.insert(client_socket);
                    break;
                }
            }
            if(!roomExists){
                chatRooms.push_back({roomnumber,username,{},"",{client_socket}});
                return std::string("Welcome to the public chat room.\n") + "Room number: " + std::to_string(roomnumber) + "\n" + "Owner: " + username + "\n";

            }
            for(auto& room : chatRooms){
                if(room.roomNumber == roomnumber){
                    for(int participant : room.participants){
                        if(participant != client_socket){
                            std::string msg = username + " had enter the chat room.\n";
                            send(participant,msg.c_str(), msg.size(), 0);
                        }
                    }
                    //return std::string("Welcome to the public chat room.\n") + "Room number: " + std::to_string(room.roomNumber) + "\n" + "Owner: " + room.owner + "\n";
                    std::string msg = std::string("Welcome to the public chat room.\n") + "Room number: " + std::to_string(room.roomNumber) + "\n" + "Owner: " + room.owner + "\n";
                    for(size_t i=0;i<room.chatHistory.size();i++){
                        msg += room.chatHistory[i];
                    }
                    msg += room.pinMessage;
                    return msg;
                    //還有chat History
                }
            }

        }
        else if(tokens[1]=="list-chat-room"){
            if(tokens.size() != 2){
                return "Usage: list-chat-room\n";
            }
            bool isLoggedIn = false;
            std::string username;
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    isLoggedIn = true;
                    username = user.username;
                    break;
                }
            }
            // If the user is not logged in
            if (!isLoggedIn) {
                return "Please login first.\n";
            }

            std::sort(chatRooms.begin(), chatRooms.end(), [](const ChatRoom& a, const ChatRoom& b) {
                return a.roomNumber < b.roomNumber;
            });

            std::string  listroom ="";
            //room should sort by the number
            for(auto& room : chatRooms){
                listroom += (room.owner + " " + std::to_string(room.roomNumber) + "\n");
            }
            return listroom;

        }
        else if(tokens[1] == "close-chat-room"){
            if(tokens.size() != 3){
                return "Usage: close-chat-room <number>\n";
            }
            int roomNumberToClose = std::stoi(tokens[2]);

            // Check if the client is logged in
            bool isLoggedIn = false;
            std::string username;
            for (auto& user : users) {
                if (user.isLoggedIn && user.clientSocket == client_socket) {
                    isLoggedIn = true;
                    username = user.username;
                    break;
                }
            }
            if (!isLoggedIn) {
                return "Please login first.\n";
            }
            bool roomExists = false;
            bool isOwner = false;
            for(auto& room : chatRooms){
                if(room.roomNumber == roomNumberToClose){
                    roomExists = true;
                    if(room.owner == username){
                        isOwner = true;
                        for(int participant : room.participants){
                            if(participant != client_socket){
                                std::string msg = "Chat room " + std::to_string(roomNumberToClose) + " was closed.\n";
                                send(participant, msg.c_str(), msg.size(), 0);
                                send(participant, "% ", 2, 0);//close room 之後participant要回到basic command mode
                                
                                
                            }//要想辦法將user的chatroom變成0
                            for(auto& user : users){
                                if(user.isLoggedIn && user.clientSocket == participant){
                                    user.chatRoomnum = 0;
                                }
                            }
                        }// Remove the chat room
                        chatRooms.erase(std::remove_if(chatRooms.begin(), chatRooms.end(),[roomNumberToClose](const ChatRoom& room) {return room.roomNumber == roomNumberToClose;}),chatRooms.end());
                        return "Chat room " + std::to_string(roomNumberToClose) + " was closed.\n";
                    }
                    
                }
            }
            if(!roomExists){
                // If the chat room does not exist
                return "Chat room " + std::to_string(roomNumberToClose) + " does not exist.\n";
            }
            if (!isOwner) {
                return "Only the owner can close this chat room.\n";
            }
            

        }
        else {
            return "Error: Unknown command\n";
        }
    }
    else{
        //[ta1]: i am waiting for everyone.
        if(tokens[0][0] != '/' && command.size()<=150){
            std::string  msg;
            msg += std::string("[" + currentuser + "]: ");
            for(size_t i=0;i<tokens.size()-1;i++){
                msg += tokens[i];
                msg += " ";
            }
            msg += tokens[tokens.size()-1];
            msg += "\n";
            for(auto& room : chatRooms){
                if(room.roomNumber == currentchatroom){
                    if(room.chatHistory.size()>=10){
                        room.chatHistory.erase(room.chatHistory.begin());
                    }
                    room.chatHistory.push_back(msg);
                    for(int participant : room.participants){
                        if(participant != client_socket){
                            send(participant, msg.c_str(), msg.size(), 0);
                        }
                    }
                    return msg;
                }
            }
        }
        else if(tokens[0] == "/pin"){
            std::string pinmsg = "Pin -> [" + currentuser + "]: ";
            for(size_t i = 1; i<tokens.size()-1;i++){
                pinmsg += tokens[i];
                pinmsg += " ";
            }
            pinmsg += tokens[tokens.size()-1];
            pinmsg += "\n";
            for (auto& room : chatRooms) {
                if (room.roomNumber == currentchatroom) {
                    room.pinMessage = pinmsg;
                    
                    // Broadcast the pinned message to all participants
                    for (int participant : room.participants) {
                        if (participant != client_socket) {
                            send(participant, pinmsg.c_str(), pinmsg.size(), 0);
                        }
                    }
                    return pinmsg;
                }
            }
        }
        else if(tokens[0] == "/delete-pin"){
            for(auto& room : chatRooms){
                if(room.roomNumber == currentchatroom){
                    if(room.pinMessage.empty()){
                        return "No pin message in chat room " + std::to_string(currentchatroom) + "\n";
                    }
                    else{
                        room.pinMessage = "";
                        return "";
                    }
                }
            }
        }
        else if(tokens[0] == "/exit-chat-room"){
            for(auto& user : users){
                if(user.username == currentuser){
                    user.chatRoomnum = 0;
                }
            }
            std::string msg = currentuser + " had left the chat room.\n";
            for(auto& room : chatRooms){
                if(room.roomNumber == currentchatroom){
                    for(int participant : room.participants){
                        if(participant != client_socket){
                            send(participant, msg.c_str(), msg.size(), 0);
                        }
                    }
                    room.participants.erase(client_socket);
                }
            }
            return "";
        }
        else if(tokens[0] == "/list-user"){
            std::string msg = "";
            for(auto& user : users){
                if(user.chatRoomnum == currentchatroom){
                    msg += std::string(user.username + " " + user.status + "\n");
                }
            }
            return msg;
        }
        else{
            return "Error: Unknown command\n"; 
        }
        
    }
    
    //return ""; // Default empty response
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [port number]" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_socket, client_socket, max_sd, activity;
    sockaddr_in address;
    fd_set readfds;
    std::vector<int> client_sockets(MAX_CLIENTS, 0);
    char buffer[BUFFER_SIZE] = {0};

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    listen(server_socket, MAX_CLIENTS);
    //std::cout << "Server started. Waiting for connections on port " << port << "..." << std::endl;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            client_socket = client_sockets[i];
            if (client_socket > 0) {
                FD_SET(client_socket, &readfds);
            }
            if (client_socket > max_sd) {
                max_sd = client_socket;
            }
        }

        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
        
        if (FD_ISSET(server_socket, &readfds)) {
            client_socket = accept(server_socket, nullptr, nullptr);
            send_welcome_message(client_socket);
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_socket;
                    send(client_socket, "% ", 2, 0);
                    break;
                }
            }
        }
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            client_socket = client_sockets[i];
            if (FD_ISSET(client_socket, &readfds)) {
                int valread = read(client_socket, buffer, BUFFER_SIZE);
                if (valread == 0) {//如果沒有讀到command
                    close(client_socket);
                    client_sockets[i] = 0;
                    continue;
                } else {
                    // Echo received data back to client
                    buffer[valread] = '\0';
                    std::string command1(buffer);
                    std::string command2 = filterMessage(command1);
                    //std::string command = "% "+command2;
                    //***
                    bool inRoom = false;
                    for(auto& user : users){
                        if(user.clientSocket == client_socket && user.chatRoomnum != 0){
                            inRoom = true;
                        }
                    }
                    if(!inRoom){
                        std::string command = "% "+command2;
                        std::string response = handle_command(client_socket, command);
                        send(client_socket, response.c_str(), response.size(), 0);
                    }else{
                        std::string response = handle_command(client_socket, command2);
                        send(client_socket, response.c_str(), response.size(), 0);
                    }
                    //***
                    //std::string response = handle_command(client_socket, command);
                    //send(client_socket, response.c_str(), response.size(), 0);
                    // Clear the buffer
                    memset(buffer, 0, BUFFER_SIZE);
                    if(exit_true){
                        close(client_socket);
                        client_sockets[i] = 0;
                        exit_true = false;
                        //break;
                    }
                    //***
                    bool inRoom1 = false;
                    for(auto& user : users){
                        if(user.clientSocket == client_socket && user.chatRoomnum != 0){//這個client還是login的狀態且還在某個聊天室裡
                            inRoom1 = true;
                        }
                    }
                    if(!inRoom1){
                        send(client_socket, "% ", 2, 0);
                    }
                    //***
                }
            }
        }
    }
    for(int client : client_sockets){
        std::cout<<"33333"<<std::endl;
        close(client);
    }
    close(server_socket);
    return 0;
}
