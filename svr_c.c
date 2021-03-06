/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr

#include "utilities.h"

int main(int argc , char *argv[])
{


    int sock;
    struct sockaddr_in server;
    char message[BUFSIZE] , server_reply[BUFSIZE];

    memset(message,'\0', BUFSIZE);
    memset(server_reply,'\0', BUFSIZE);

    //Create socket
    if ((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 20856 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    while(1){
        printf("Enter message : ");
        fgets(message, BUFSIZE, stdin);

        //Send some data
        if( send(sock, message, BUFSIZE, 0) < 0){
            puts("Send failed");
            return 1;
        }
        memset(message,'\0', BUFSIZE);

        //Receive a reply from the server
        if( recv(sock, server_reply, BUFSIZE, 0) < 0){
            puts("recv failed");
            break;
        }

        puts(server_reply);
        memset(server_reply,'\0', BUFSIZE);
    }

    close(sock);
    return 0;
}
