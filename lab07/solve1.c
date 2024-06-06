/*#include <stdio.h>

int main(){
    printf("hello");
    return 0;
}*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>


#define SERVER_PATH "/queen.sock"
#define N 30
char puzzle[30][30];
int check_row[30];
int check_col[30];
int dig_up[60];
int dig_down[60];

int isSafe(int row, int col)
{
   return (!check_col[col] && !check_row[row] && !dig_down[col-row+N-1] && !dig_up[col+row]);
}

int solveNQUtil(int client_socket,int row,char board[N][N])
{
    // Base case: If all queens are placed
    // then return true
    if (row >= N){//col >= N
        printf("11111\n");
        for(int i=0;i<N;i++){
            for(int w=0;w<N;w++){
                puzzle[i][w]=board[i][w];
            }
        }
        return 1;
    }
    if(check_row[row] != 0){
        return solveNQUtil(client_socket,row + 1,board);
    }
 
    
    for (int i = 0; i < N; i++) {
        if (isSafe(row,i)) {
             
            board[row][i] = 'Q';
            check_col[i]=1;
            check_row[row]=1;
            dig_down[i-row+N-1]=1;
            dig_up[i+row]=1;

            if (solveNQUtil(client_socket,row + 1,board))
                return 1;

            board[row][i] = '.'; // BACKTRACK
            check_col[i]=0;
            check_row[row]=0;
            dig_down[i-row+N-1]=0;
            dig_up[i+row]=0;
        }
    }
 
    return 0;
}
int main() {
    // Create a UNIX domain socket
    printf("HELLO\n");
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SERVER_PATH, sizeof(server_address.sun_path) - 1);

    // Connect to the challenge server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }


    // Send the "S" command to the server
    char command[] = "S";
    if (send(client_socket, command, strlen(command), 0) == -1) {
        perror("Error sending command");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    char response[4096];
    // Receive and print the response
    if (recv(client_socket, response, sizeof(response), 0) == -1) {
        perror("Error receiving response");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    //printf("%s\n", response);
    //char puzzle[30][30];
    char response1[4096];
    int k=0;
    for(int i=0; i<strlen(response);i++){
        if(response[i]=='.' || response[i] =='Q'){
            response1[k]=response[i];
            k++;
        }
    }
    //printf("%s\n", response1);
    for(int i=0;i<30;i++){
        check_col[i]=0;
        check_row[i]=0;
    }
    for(int i=0;i<60;i++){
        dig_down[i]=0;
        dig_up[i]=0;
    }
    int p=0;
    for(int i=0;i<30;i++){
        for(int q=0;q<30;q++){
            puzzle[i][q]=response1[p];
            if(puzzle[i][q]=='Q'){
                check_col[q]=1;
                check_row[i]=1;
                dig_down[q-i+N-1]=1;
                dig_up[q+i]=1;
            }
            p++;
        }
    }
    
    if (solveNQUtil(client_socket,0,puzzle) == false) {
        printf("Solution does not exist");
        return 0;
    }
    for(int i=0;i<N;i++){
        for(int w=0;w<N;w++){
            if(puzzle[i][w]=='Q'){
                char com[20]; // Changed size to accommodate larger commands
                snprintf(com, sizeof(com), "M %d %d\n", i, w);
                ssize_t bytes_written = write(client_socket, com, strlen(com));
                usleep(500);
            }
        }
    }
    // Send the "C" command to the server
    char command1[5];
    snprintf(command1, sizeof(command1), "C");
    write(client_socket, command1, strlen(command1));
    usleep(500);

    // Close the connection
    close(client_socket);

    return 0;
}
