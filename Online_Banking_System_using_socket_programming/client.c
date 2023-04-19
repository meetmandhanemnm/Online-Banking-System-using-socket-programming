#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int client_socket;

    client_socket = socket(AF_INET,SOCK_STREAM,0);
    if(client_socket==-1) {
        perror("Error while creating client socket");
        _exit(0);
    }
    else {
        struct sockaddr_in serverAddress,clientAddress;

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8081);
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

        int connectStatus = connect(client_socket,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
        if(connectStatus==-1) {
            perror("Error while connecting to server");
            close(client_socket);
            _exit(0);
        }
        else {
            printf("\n\n*******Welcome to  Online banking management system ******\n");

            char ch;
            
            printf("\n\nEnter the login account type\n1. Admin\n2. Customer \n\nEnter your choice(1 or 2) : ");
            scanf("%c", &ch);
            if (ch != '1' && ch != '2') {
                printf("Not a vaild input\n");
                exit(0);
            }

            write(client_socket, &ch, 1);
            printf("\n----------------------------------------------------------\n");

            char readBuffer[1000], writeBuffer[1000];
            ssize_t readBytes, writeBytes;           

            char tempBuffer[1000];

            do {
                bzero(readBuffer, sizeof(readBuffer));
                bzero(tempBuffer, sizeof(tempBuffer));
                readBytes = read(client_socket, readBuffer, sizeof(readBuffer));
                if (readBytes <= 0)
                    perror("Error while reading from client socket!");
                else if (strchr(readBuffer, '^') != NULL) {
                    // Skip read from client
                    strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
                    printf("%s", tempBuffer);
                    writeBytes = write(client_socket, "^", strlen("^"));
                    if (writeBytes == -1) {
                        perror("Error while writing to client socket!");
                        break;
                    }
                }
                else if (strchr(readBuffer, '$') != NULL) {
                    // Server sent an error message and is now closing it's end
                    strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 2);
                    printf("%s", tempBuffer);
                    printf("\n\nClosing the connection to the server now!\n\n");
                    exit(0);
                }
                else {

                    bzero(writeBuffer, sizeof(writeBuffer)); // Empty the write buffer
                    // password input
                    if (strchr(readBuffer, '>') != NULL)
                        strcpy(writeBuffer, getpass(readBuffer));
                    else {
                        // data input
                        printf("%s", readBuffer);
                        scanf("%s", writeBuffer); // Take user input!
                    }

                    writeBytes = write(client_socket, writeBuffer, strlen(writeBuffer));
                    if (writeBytes == -1) {
                        perror("Error while writing to client socket!");
                        printf("Closing the connection to the server now!\n");
                        break;
                    }
                }
            } while (readBytes > 0);

            close(client_socket);

        }
    }
}