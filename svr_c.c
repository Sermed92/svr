/*  svr_c.c
*   Implementacion del ATM (cliente)
*   Autores:
*   Sergio Medina 09-11259
*   Lucio Mederos 13-10856
*/
#include "utilities.h"
/* 
*   Funcion unica para el funcionamiento del cliente
*   Ejemplo de llamada:
*   ./svr_c -d 127.0.0.1 -p 20856
*/
int main(int argc , char *argv[]){

    // Se verifica la cantidad de argumentos
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

    // se inicializan los buffers, para que esten limpios
    memset(message,'\0', BUFSIZE);
    memset(buffer_to_send, '\0', BUFSIZE);

    // Creacion del socket a usar, protocolo tcp == SOCK_STREAM
    if ((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    // Se prepara el servidor
    server.sin_family = AF_INET;
    int value;
    // flags indican que opcines se tomaron
    int dflag = 0;
    int pflag = 0;
    int lflag = 0;
    // Se capturan los argumentos
    while ((value = getopt(argc, argv, "d:p:l:")) != -1){
        switch (value) {
            case 'd':
                dflag = 1;
                // se agina la direccion
                server.sin_addr.s_addr = inet_addr(optarg);
            break;
            case 'p':
                pflag = 1;
                // se asigna el puerto
                server.sin_port = htons(atoi(optarg));
            break;
            case 'l':
                lflag = 1;
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

    // si se escoge la opcion -l pero no -d o -p se termina la ejecucion
    if ((lflag == 1) && (dflag != 1 || pflag != 1)){
        printf("Opciones -d y -p son mandatorias\n");
        exit(1);
    }

    // Conectamos con el servidor
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    // En constante comunicacion con el servidor
    // Mientras no se cierre el cliente
    while(1){

        printf("Send Report: ");
        // se obtiene el reporte a enviar
        fgets(message, BUFSIZE, stdin);
        // se obtiene el codigo referente al reporte
        message_code = get_message_code(message);

        //Get current time
        time(&current_time);
        time_struct = localtime(&current_time);
        // se le asigna un formato al 'tiempo' actual
        if(strftime(time_buffer, 50, "%H.%M.%S:%d.%m.%Y", time_struct) == 0) {
          perror("Error! Could not prepare time string");
          return 0;
        }

        // se prepara el buffer a ser enviado al servidro
        sprintf(buffer_to_send, "ATM-ID:%d|%s|Cod:%d|Message: %s", self_id, time_buffer, message_code, message);

        // Envio de datos al servidor
        if(send(sock, buffer_to_send, BUFSIZE, 0) < 0){
            puts("Send failed");
            return 1;
        }
        // se limpia el buffer
        memset(message,'\0', BUFSIZE);

        // Recibe una respuesta del servidor
        if( recv(sock, message, BUFSIZE, 0) < 0){
            // La respuesta no se recibio
            puts("recv failed");
            break;
        }

        // Se limpian los buffer usados
        memset(message,'\0', BUFSIZE);
        memset(buffer_to_send, '\0', BUFSIZE);
        memset(time_buffer, '\0', 50);
    }

    close(sock);
    return 0;
}
