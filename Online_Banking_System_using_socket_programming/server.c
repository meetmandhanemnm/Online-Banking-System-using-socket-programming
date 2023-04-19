#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "header/constants.h"
#include "header/admin.h"
#include "header/customer.h"

int main() {
    int server_socket,client_socket;
    
    server_socket = socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1) {
        perror("Error while creating socket");
        exit(0);
    }
    else {
        struct sockaddr_in serverAddress,clientAddress;

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8081);
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        int bindStatus = bind(server_socket,(struct sockaddr *)&serverAddress, sizeof(serverAddress));
        if(bindStatus==-1) {
            perror("Error while binding the socket");
            exit(0);
        }
        else {
            int listenStatus = listen(server_socket,5);
            if(listenStatus==-1) {
                perror("Error while listening to client connections");
                close(server_socket);
                exit(0);
            }
            else {
                int clientSize;
                printf("Waiting for clients to connect\n");
                while(true) {
                    clientSize = (int)sizeof(clientAddress);
                    client_socket = accept(server_socket, (struct sockaddr *)&clientAddress, &clientSize);
                    if(client_socket==-1) {
                        perror("Error while connecting to client");
                        close(server_socket);
                        exit(0);
                    }
                    else {
                        int fork_return = fork();
                        if(fork_return==0) {

                            printf("Client has connected to the server!\n");
                            char readBuffer[1000], writeBuffer[1000];
                            ssize_t readBytes, writeBytes;
                            int choice;
                            bzero(readBuffer, sizeof(readBuffer));
                            readBytes = read(client_socket, readBuffer, sizeof(readBuffer));
                            if (readBytes == -1)
                                perror("Error while reading from client");
                            else if (readBytes == 0)
                                printf("No data was sent by the client");
                            else
                            {
                                choice = atoi(readBuffer);
                                switch (choice)
                                {
                                    case 1:
                                        admin_handler(client_socket);
                                        break;
                                    case 2:
                                        customer_handler(client_socket);
                                        break;
                                    default:
                                        break;
                                }
                            }

                            printf("Terminating connection to client!\n");
                            close(client_socket);
                            _exit(0);
                        }
                    }
                }
            }
        }
    }
    close(server_socket);
    return 0;
}