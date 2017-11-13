/*
    C socket server example
*/
#include <sys/socket.h> 
#include "utilities.h"
 
int main(int argc , char *argv[]){

    argc_verify_s(argc);

    // Se definen las variables para ser capturadas como argumentos
    int port_number;
    output_file = NULL;
    f_name = NULL;

    int value;
    // Se capturan los argumentos
    while ((value = getopt(argc, argv, "l:b:")) != -1){
        switch (value) {
            case 'l':
                port_number = atoi(optarg);
            break;
            case 'b':
                f_name = optarg;
            break;
            case '?':
                if (optopt == 'l' || optopt == 'b') {
                    printf("Opcion '-%c' requiere un argumento\n", optopt);
                } else if(isprint(optopt)) {
                    printf("Opcion desconocida '-%c'\n", optopt);
                } else {
                    printf("Caracter desconocido '\\x%x'\n", optopt);
                }
                exit(1);
            default:
                exit(1);
        }
    }

    output_file = output_ready(f_name);

    int socket_desc, c, client_sock, read_size;
    struct sockaddr_in server;
    struct sockaddr_in client;
    char report_message[BUFSIZE];
    memset(report_message,'\0',BUFSIZE);
    int i;

    // Se crea el socket
    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("No se pudo crear el socket\n");
        exit(1);
    }

    // Se llena la estructura sockaddr_in para server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons (port_number);

    //Bind
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        //print the error message
        perror("bind failed. Error");
        return 1;
    }    

    //Listen
    listen(socket_desc , 3);    

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);    


    printf("Puerto %d\n", port_number);
    printf("Nombre %s\n", f_name);    

    //accept connection from an incoming client (aqui es donde sucede la magia con el cliente)
    if ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) < 0){
        perror("accept failed");
        return 1;        
    }

    puts("Connection accepted");
     
    //Receive a message from client
    i=1;
    while((read_size = recv(client_sock, report_message, BUFSIZE, 0)) > 0){
        //Send the message back to client
        write(client_sock, report_message, strlen(report_message));
        printf("Message %d\n",i);
        fflush(stdout);
        memset(report_message,'\0',BUFSIZE);
        i++;
    }
     
    if(read_size == 0){
        puts("Client disconnected");
        fflush(stdout);
    } else if(read_size == -1){
        perror("recv failed");
    }
     
    return 0;
}
