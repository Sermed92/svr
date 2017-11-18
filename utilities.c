/*	utilities.c
*	Archivo con la implementacion de funciones requeridas
*	Autores:
*	Sergio Medina 09-11259
*	Lucio Mederos 13-10856
*/
#include "utilities.h"

// Definicion de mensajes base
const char *FILE_OPEN_FAIL = "Error! - No se pudo abrir archivo";
const char *WRONG_ARGUMENT_S = "Error!\n Uso de svr_s:\n./svr_s -l <puerto_svr_s> -b <archivo_bitacora>";
const char *WRONG_ARGUMENT_C = "Error!\n Uso de svr_c:\n./svr_c -d <nombre_modulo_central> -p <puerto_svr_s> [-l <puerto_local>]";


// Se verifica la cantidad de argumentos recibida para el servidor
// int amount: Cantidad de argumentos
// Retorna: void
void argc_verify_s(int amount) {
	if (amount != 5) {
		printf("%s\n", WRONG_ARGUMENT_S);
		exit(1);
	}
}

// Se verifica la cantidad de argumentos recibida para el cliente
// int amount: Cantidad de argumentos
// Retorna: void
void argc_verify_c(int amount) {
	if (amount != 5) {
		if (amount != 7){
			printf("%s\n", WRONG_ARGUMENT_C);
			exit(1);
		}
	}
}

// Se abre el archivo a usar como bitacora en modo append
// char * f_name: Nombre del archivo a abrir
// Retorna: File* Archivo abierto
FILE *output_ready(char* f_name){

	if ((output_file = fopen(f_name, "a")) == NULL) {
		printf("%s %s\n",FILE_OPEN_FAIL, f_name);
		exit(1);
	}
	else {
		return output_file;
	}
}

// Manejador de señales para evitar el cierre forzoso del programa
// int sig_num: Argumento de la señal de interrupcion
// Retorna: void
void sigintHandler(int sig_num){
    signal(sig_num, sigintHandler);
	char option;
    printf("\n Deseas finalizar el programa? [s/n]");
	fflush(stdout);
	option = getchar();

    if (option=='s'){
    	// se indico que se quiere terminar
		ending_server = true;
		while (true){
			printf("Saliendo\n");
			exit(0);
		}
	} else {
		// no se quiere terminar
		printf("No salgo\n");
	}
}

// Si es necesario enviar una alarma, indica el codigo de la alarma
// char *buffer: mensaje recibido a ser revisado para obtener su codigo
// Retorna: int Codigo correspondiente al mensaje recibido (0 si es un mensaje desconocido)
int get_message_code(char* buffer) {
	// arreglo con las alarmas que se deben detectar
	// la posicion (+1) de cada mensaje es su codigo de alarma
	char alert_needed[][BUFSIZE] = {"empty\n", "Printer Error\n", "Communication Offline\n",
		"Communication error\n", "Low Cash alert\n", "Running Out of notes in cassette\n",
		"Service mode entered\n", "Service mode left\n", "device did not answer as expected\n",
		"The protocol was cancelled\n", "Low Paper warning\n", "Paper-out condition\n"
	};

	for (int i = 0; i < 12; i++) {
		if (strncmp(alert_needed[i], buffer, BUFSIZE) == 0) {
			// Se detecto una alarma ==> se devuelve el codigo de alarma
			return (i+1);
		}
	}
	// si no se detecta alarma, el codigo de la operacion es 0, que significa success
	return 0;
}

// Funcion llamada por cada hilo para atender varios clientes
// void *socket_desc: Descriptor del socket correspondiente al hilo
// Retorna: void
void *connection_handler(void *socket_desc) {
	int socket = *(int*) socket_desc;
	int read_size;
	int message_code = -1;
	char report_message[BUFSIZE];

	struct timeval tv;
	tv.tv_sec = 5*60; // Timeout de 5 minutos
	tv.tv_usec = 0;
	// Se usa la llamada al sistema setsockopt para establecer el timeout en el socket
	if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval)) != 0) {
		perror("Error al establecer timeout en socket");
	}

	while (1) {
		while((read_size = recv(socket, report_message, BUFSIZE, 0)) > 0){
		// Se indico que se quiere cerrar el servidor (ctrl+c)
		if (ending_server) {
			return 0;
		}

		// Chequear si necesita enviar email de alarma
		char * message = strchr(report_message, ' ')+1;
		message_code = get_message_code(message);
		if (message_code != 0) {
			// Si el codigo de la transaccion no es 0 (success)
			//se envia la alarma con su respectivo codigo
			email_alarm(message, message_code);
		}

		write(socket, "Accepted\0", strlen("Accepted\0"));
		fflush(stdout);

		//Escribir en bitacora
		//Controlar con semaforo -- region critica
		sem_wait(&semaphore);

		output_file = output_ready(f_name);

		fprintf(output_file, "%s", report_message);
		printf("%s", report_message);

		if (fclose(output_file) != 0){
			perror("Error al cerrar el archivo");
		}

		sem_post(&semaphore);
		//Salida de la region critica

		// Se limpia el buffer
		memset(report_message, '\0', BUFSIZE);

	}

	if(read_size == 0){
	    puts("Client disconnected");
	    connection_counter--;
	    fflush(stdout);
	} else if(read_size == -1){
		printf("ALARMA! Cliente inactivo durante 5 minutos\n");
		email_alarm("ALARMA! Cliente inactivo durante 5 minutos\n", 13);
	}
}

	return 0;
}

// Funcion para enviar un correo de alarma
// char *report: Mensaje recibido a ser reenviado por email
// int code: code de alarma
// Retorna: void
void email_alarm(char *report, int code) {
	// se reporta la alarma en el log del server
	printf("ALARMA! Reporte (%d) detectado: ", code);
	char cmd[BUFSIZE];  					// comando a enviar al sistema
	char to[] = "sermed19@gmail.com"; 		// email destinatario
	char body[BUFSIZE];	    				// mensajea  enviar
	char tempFile[] = "alarm-email";    	// nombre del mensaje a enviar
	// se copia el reporte en el cuerpo del mensaje
	strcpy(body,report);

	// se abre el archivo para escribir, si ya existe lo reescribe desde el inicio
	FILE *fp = fopen(tempFile,"w");	// open it for writing.
	fprintf(fp,"%s\n",body);
	fclose(fp);

	sprintf(cmd,"mail %s < %s",to,tempFile); // se crea el comando a ejecutar
	// se usa la syscall "system" para usar el comando
	system(cmd);
}
