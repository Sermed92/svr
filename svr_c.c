/*
    C ECHO client example using sockets
*/
#include "utilities.h"

int main(int argc , char *argv[]){

    // ./svr_c -d 127.0.0.1 -p 21259
    argc_verify_c(argc);

    //Set self atm id
    srand(time(NULL));
    int self_id = rand();
    if (self_id < 0) {
      self_id = self_id * -1;
    }

    int sock;
    int message_code;
    struct sockaddr_in server;
    char message[BUFSIZE];
    time_t current_time;
    struct tm * time_struct;
    char time_buffer[50];
    char buffer_to_send[BUFSIZE];

    memset(message,'\0', BUFSIZE);
    memset(buffer_to_send, '\0', BUFSIZE);

    //Create socket
    if ((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    //server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    //server.sin_port = htons( 20856 );

    int value;
    int dflag = 0;
    int pflag = 0;
    int lflag = 0;
    // Se capturan los argumentos
    while ((value = getopt(argc, argv, "d:p:l:")) != -1){
        switch (value) {
            case 'd':
                dflag = 1;
                server.sin_addr.s_addr = inet_addr(optarg);
            break;
            case 'p':
                pflag = 1;
                server.sin_port = htons(atoi(optarg));
            break;
            case 'l':
                lflag = 1;
                server.sin_port = htons(atoi(optarg));
            break;
            case '?':
                if (optopt == 'd' || optopt == 'p' || optopt == 'l') {
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

    if ((lflag == 1) && (dflag != 1 || pflag != 1)){
        printf("Opciones -d y -p son mandatorias\n");
        exit(1);
    }

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    while(1){
        printf("Send Report: ");
        fgets(message, BUFSIZE, stdin);

        message_code = get_message_code(message);

        //Get current time
        time(&current_time);
        time_struct = localtime(&current_time);

        if(strftime(time_buffer, 50, "%H.%M.%S:%d.%m.%Y", time_struct) == 0) {
          perror("Error! Could not prepare time string");
          return 0;
        }

        sprintf(buffer_to_send, "ATM-ID:%d|%s|Cod:%d|Mes:%s", self_id, time_buffer, message_code, message);

        //Send some data
        if(send(sock, buffer_to_send, BUFSIZE, 0) < 0){
            puts("Send failed");
            return 1;
        }
        memset(message,'\0', BUFSIZE);

        //Receive a reply from the server
        if( recv(sock, message, BUFSIZE, 0) < 0){
            puts("recv failed");
            break;
        }

        //puts(server_reply);
        memset(message,'\0', BUFSIZE);
        memset(buffer_to_send, '\0', BUFSIZE);
        memset(time_buffer, '\0', 50);
    }

    close(sock);
    return 0;
}
