/*
    C socket server example
*/
#include "utilities.h"

int main(int argc , char *argv[]){

    // ./svr_s -l 21259 -b bitacora
    argc_verify_s(argc);
    // Se previene la interrupcion CTRL+C
    signal(SIGINT, sigintHandler);
    // Se definen las variables para ser capturadas como argumentos
    int port_number;
    output_file = NULL;
    f_name = NULL;
    ending_server=false;

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

    // time_t current_time;
    int socket_desc, c, client_sock, *new_socket;
    struct sockaddr_in server;
    struct sockaddr_in client;
    char report_message[BUFSIZE];
    memset(report_message,'\0',BUFSIZE);
    int connection_counter = 0;

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

    if(sem_init(&semaphore, 0, 1) != 0) {
      perror("Semaphore initialization failed\n");
    }

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client (aqui es donde sucede la magia con el cliente)
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
            && connection_counter < 1000){
    printf("Connection accepted from device %d\n", connection_counter);

    pthread_t sniffer_thread;
    new_socket = malloc(1);
    *new_socket = client_sock;

    if(pthread_create(&sniffer_thread, NULL, connection_handler, (void*)new_socket)) {
      perror("Could not create thread");
      return 1;
    }

      printf("Handling thread created\n");
    }

    if (fclose(output_file) != 0){
      perror("Error al cerrar el archivo");
    }

    return 0;
}
